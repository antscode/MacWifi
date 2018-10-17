#include <Events.h>
#include <Resources.h>
#include <string.h>
#include "WifiShared.h"
#include "MacWifi.h"
#include "Prefs.h"
#include "WifiEvents.h"
#include "Modules\VM300.h"
#include "Modules\OpenWRT.h"

WifiModule* _wifiModule;
WifiData* _sharedDataPtr;
Prefs _prefs;

int main()
{	
	EventRecord event;
	
	EventInit();
	GetSharedData();
	GetPrefs();	
	GetWifiModule();

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
			switch (_sharedDataPtr->Status)
			{
				case ScanRequest:
					_sharedDataPtr->Status = Scanning;
					_sharedDataPtr->Error = false;

					_wifiModule->GetNetworks();
					break;

				case ConnectRequest:
					_sharedDataPtr->Status = Connecting;
					_sharedDataPtr->Error = false;

					_wifiModule->Connect(
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

void GetWifiModule()
{
	switch (_sharedDataPtr->Device)
	{
		case 2:
			_wifiModule = new OpenWRT;
			break;

		default:
			_wifiModule = new VM300;
			break;
	}

	_wifiModule->WifiDataPtr = _sharedDataPtr;
}

void EventInit()
{
	AEInstallEventHandler(
		kCoreEventClass,
		kAEQuitApplication,
		(AEEventHandlerUPP)Quit,
		0L,
		false);

	AEInstallEventHandler(
		kWifiClass,
		kRequest,
		(AEEventHandlerUPP)ProcessRequestEvent,
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

pascal OSErr ProcessRequestEvent(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	Size actualSize;
	DescType typeCode;

	char cMethod[255];
	char cUri[255];
	char cData[1000];
	int callbackId;
	string method, url, data;

	AEGetParamPtr(appleEvent, kMethodParam, typeChar, &typeCode, cMethod, sizeof(cMethod), &actualSize);
	method.assign(cMethod, actualSize);

	AEGetParamPtr(appleEvent, kUriParam, typeChar, &typeCode, cUri, sizeof(cUri), &actualSize);
	url.assign(cUri, actualSize);

	AEGetParamPtr(appleEvent, kDataParam, typeChar, &typeCode, cData, sizeof(cData), &actualSize);
	data.assign(cData, actualSize);

	AEGetParamPtr(appleEvent, kCallbackIdParam, typeInteger, &typeCode, &callbackId, sizeof(int), &actualSize);

	_requestComplete = false;

	// Initialise an SSL tunnel if required
	InitTunnel(url);

	if (method == "GET")
	{
		Comms::Http.Get(url, RequestComplete);
	}
	else if (method == "POST")
	{
		Comms::Http.Post(url, data, RequestComplete);
	}

	while (!_requestComplete)
	{
		Comms::Http.ProcessRequests();
	}
	
	const char* cErrorMsg = _response.ErrorMsg.c_str();
	const char* cContent = _response.Content.c_str();

	AEPutParamPtr(reply, kCallbackIdParam, typeInteger, &callbackId, sizeof(int));
	AEPutParamPtr(reply, kSuccessParam, typeBoolean, &_response.Success, sizeof(bool));
	AEPutParamPtr(reply, kStatusCodeParam, typeInteger, &_response.StatusCode, sizeof(int));
	AEPutParamPtr(reply, kErrorMsgParam, typeChar, cErrorMsg, strlen(cErrorMsg));
	AEPutParamPtr(reply, kContentParam, typeChar, cContent, strlen(cContent));

	return noErr;
}

void InitTunnel(string requestUri)
{
	try
	{
		Uri uri(requestUri);

		if (uri.Scheme == "https")
		{
			//_wifiModule->GetTunnel(uri.Host, function<void(string, int)> onComplete));
		}
	}
	catch (const invalid_argument& e)
	{
		// TODO: handle error
	}
}

void RequestComplete(HttpResponse response)
{
	_requestComplete = true;
	_response = response;
}

pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_run = false;
}