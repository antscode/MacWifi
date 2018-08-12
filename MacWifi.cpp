#include <Events.h>
#include <Resources.h>
#include <string.h>
#include "WifiShared.h"
#include "MacWifi.h"
#include "Prefs.h"
#include "Modules\VM300.h"
#include "Modules\OpenWRT.h"

WifiData* _sharedDataPtr = 0;
Prefs _prefs;

int main()
{	
	EventRecord event;
	WifiModule* wifiModule;

	EventInit();
	GetSharedData();
	GetPrefs();	

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

						wifiModule = GetWifiModule();
						wifiModule->GetNetworks();
						break;

					case ConnectRequest:
						_sharedDataPtr->Status = Connecting;
						_sharedDataPtr->Error = false;

						wifiModule = GetWifiModule();
						wifiModule->Connect(
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

					case SavePrefsRequest:
						SavePrefs();
						_sharedDataPtr->Status = ScanRequest;
						_sharedDataPtr->Error = false;
						_sharedDataPtr->UpdateUI = true;
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

		RemoveResource((Handle)memHandle);
		UpdateResFile(CurResFile());
		ReleaseResource((Handle)memHandle);
	}
}

void GetPrefs()
{
	memset(_sharedDataPtr->Hostname, 0, sizeof(_sharedDataPtr->Hostname));
	memset(_sharedDataPtr->Username, 0, sizeof(_sharedDataPtr->Username));
	memset(_sharedDataPtr->Password, 0, sizeof(_sharedDataPtr->Password));

	if (_prefs.Data.isMember("device"))
	{
		_sharedDataPtr->Device = _prefs.Data["device"].asInt();
		strcpy(_sharedDataPtr->Hostname, _prefs.Data["hostname"].asCString());
		strcpy(_sharedDataPtr->Username, _prefs.Data["username"].asCString());
		strcpy(_sharedDataPtr->Password, _prefs.Data["password"].asCString());
	}
	else
	{
		// Default to Vonets default settings
		_sharedDataPtr->Device = 1;
		strcpy(_sharedDataPtr->Hostname, "vonets.cfg");
		strcpy(_sharedDataPtr->Username, "admin");
		strcpy(_sharedDataPtr->Password, "admin");
	}
}

WifiModule* GetWifiModule()
{
	WifiModule* wifiModule;

	switch (_sharedDataPtr->Device)
	{
		case 2:
			wifiModule = new OpenWRT;
			break;

		default:
			wifiModule = new VM300;
			break;
	}

	wifiModule->WifiDataPtr = _sharedDataPtr;
	return wifiModule;
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

void SavePrefs()
{
	_prefs.Data["device"] = _sharedDataPtr->Device;
	_prefs.Data["hostname"] = _sharedDataPtr->Hostname;
	_prefs.Data["username"] = _sharedDataPtr->Username;
	_prefs.Data["password"] = _sharedDataPtr->Password;
	_prefs.Save();
}

pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_run = false;
}