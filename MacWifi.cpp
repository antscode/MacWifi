#include <Events.h>
#include <Resources.h>
#include <string.h>
#include "WifiShared.h"
#include "MacWifi.h"

WifiData* _sharedDataPtr;
VM300 _wifiModule;

int main()
{	
	EventRecord event;
	
	int sleep = 60;

	EventInit();
	GetSharedData();

	while (_run)
	{
		if (WaitNextEvent(everyEvent, &event, sleep, NULL))
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
					_wifiModule.Connect(_sharedDataPtr->ConnectSSID, WPA2PSK, AES, _sharedDataPtr->ConnectPwd);
					break;
			}

			Comms::Http.ProcessRequests();
		}
	}
}

void GetSharedData()
{
	MemLocHandle memHandle = (MemLocHandle)Get1Resource('memr', 128);
	_sharedDataPtr = (WifiData*)**memHandle;
	_wifiModule.WifiDataPtr = _sharedDataPtr;
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