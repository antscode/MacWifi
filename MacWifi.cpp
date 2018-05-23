#include <Events.h>
#include <Resources.h>
#include <string.h>
#include "WifiShared.h"
#include "MacWifi.h"

WifiData* _sharedDataPtr = 0;
VM300 _wifiModule;

int main()
{	
	EventRecord event;
	
	EventInit();
	GetSharedData();

	while (_run)
	{
		if (WaitNextEvent(everyEvent, &event, 0, NULL))
		{
			switch (event.what)
			{
				case kHighLevelEvent:
					AEProcessAppleEvent(&event);
					break;
			}
		}
		else
		{
			if (_sharedDataPtr == 0)
			{
				GetSharedData();
			}
			else
			{
				switch (_sharedDataPtr->Status)
				{
					case ScanRequest:
						_sharedDataPtr->Status = Scanning;
						_sharedDataPtr->Error = false;
						_wifiModule.GetNetworks();
						break;

					case ConnectRequest:
						_sharedDataPtr->Status = Connecting;
						_sharedDataPtr->Error = false;
						_wifiModule.Connect(
							std::string(_sharedDataPtr->ConnectSSID),
							_sharedDataPtr->ConnectMode,
							_sharedDataPtr->ConnectEncryption,
							std::string(_sharedDataPtr->ConnectPwd));
						break;
				}

				Comms::Http.ProcessRequests();
			}
		}
	}
}

void GetSharedData()
{
	MemLocHandle memHandle = (MemLocHandle)Get1Resource('memr', 128);

	if (ResError() != resNotFound)
	{
		_sharedDataPtr = (WifiData*)**memHandle;
		_wifiModule.WifiDataPtr = _sharedDataPtr;

		RemoveResource((Handle)memHandle);
		UpdateResFile(CurResFile());
		ReleaseResource((Handle)memHandle);
	}
}

void EventInit()
{
	AEInstallEventHandler(
		kCoreEventClass,
		kAEQuitApplication,
		(AEEventHandlerUPP)Quit,
		0L,
		false);
}

pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_run = false;
}