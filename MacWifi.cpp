#include <Events.h>
#include <Resources.h>
#include <string.h>
#include "WifiShared.h"
#include "MacWifi.h"

WifiData* _sharedDataPtr = 0;
OpenWRT _wifiModule; // VM300 _wifiModule;

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
							std::string(_sharedDataPtr->ConnectName),
							std::string(_sharedDataPtr->ConnectId),
							_sharedDataPtr->ConnectMode,
							_sharedDataPtr->ConnectEncryption,
							std::string(_sharedDataPtr->ConnectPwd));
						break;

					case RestartRequest:
						_sharedDataPtr->Status = Restarting;
						_sharedDataPtr->Error = false;
						Restart();
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

void Restart()
{
	AEDesc finderAddr = { typeNull, nil };
	AppleEvent myShutDown = { typeNull, nil };
	AppleEvent nilReply = { typeNull, nil };
	OSErr errCode;
	OSType finderSig = 'MACS';

	errCode = AECreateDesc(typeApplSignature, &finderSig, sizeof(OSType), &finderAddr);

	if (noErr == errCode) {
		errCode = AECreateAppleEvent(kAEFinderEvents, kAERestart,
			&finderAddr, kAutoGenerateReturnID,
			kAnyTransactionID, &myShutDown);
	}

	if (noErr == errCode) {
		errCode = AESend(&myShutDown, &nilReply,
			kAENoReply + kAECanSwitchLayer + kAEAlwaysInteract,
			kAENormalPriority, kAEDefaultTimeout, nil, nil);
	}

	AEDisposeDesc(&finderAddr);
	AEDisposeDesc(&myShutDown);
	AEDisposeDesc(&nilReply);
}

pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_run = false;
}