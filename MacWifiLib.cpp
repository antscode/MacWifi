#include <string.h>
#include "MacWifiLib.h"
#include "Util.h"

void MacWifiLib::Get(string requestUri, function<void(MacWifiResponse)> onComplete)
{
	SendRequestEvent("GET", requestUri, "", onComplete);
}

void MacWifiLib::Post(string requestUri, string content, function<void(MacWifiResponse)> onComplete)
{
	SendRequestEvent("POST", requestUri, content, onComplete);
}

void MacWifiLib::SendRequestEvent(string method, string uri, string content, function<void(MacWifiResponse)> onComplete)
{
	AEAddressDesc address;
	AppleEvent appleEvent;

	GetEventAddress(&address);

	AECreateAppleEvent(
		kWifiClass,
		kRequest,
		&address,
		kAutoGenerateReturnID,
		1L,
		&appleEvent);

	const char* cMethod = method.c_str();
	const char* cUri = uri.c_str();
	const char* cContent = content.c_str();

	int callbackIndex = _callbacks.size();
	_callbacks.insert(pair<int, function<void(MacWifiResponse)>>(callbackIndex, onComplete));

	AEPutParamPtr(&appleEvent, kMethodParam, typeChar, cMethod, strlen(cMethod));
	AEPutParamPtr(&appleEvent, kUriParam, typeChar, cUri, strlen(cUri));
	AEPutParamPtr(&appleEvent, kDataParam, typeChar, cContent, strlen(cContent));
	AEPutParamPtr(&appleEvent, kCallbackIdParam, typeInteger, &callbackIndex, sizeof(int));

	OSErr err = SendEvent(&appleEvent);

	if (err != noErr)
	{
		MacWifiResponse response;
		response.Success = false;
		response.ErrorMsg = "Cound not send event due to error " + to_string(err);
		onComplete(response);
	}
}

void MacWifiLib::GetEventAddress(AEAddressDesc* address)
{
	OSType appSig = 'MWFI';

	AECreateDesc(
		typeApplSignature,
		(Ptr)(&appSig),
		(Size)sizeof(appSig),
		address);
}

OSErr MacWifiLib::SendEvent(AppleEvent* appleEvent)
{
	AppleEvent reply;
	OSErr err = AESend(
		appleEvent,
		&reply,
		kAEQueueReply, 
		kAENormalPriority,
		kAEDefaultTimeout, nil, nil);

	return err;
}

OSErr MacWifiLib::ProcessReply(AppleEvent* appleEvent)
{
	Size actualSize;
	DescType typeCode;

	char cError[1000];
	char cContent[5000];

	bool success;
	int statusCode;
	int callbackId;
	string errorMsg, content;

	AEGetParamPtr(appleEvent, kCallbackIdParam, typeInteger, &typeCode, &callbackId, sizeof(int), &actualSize);
	AEGetParamPtr(appleEvent, kSuccessParam, typeBoolean, &typeCode, &success, sizeof(bool), &actualSize);
	AEGetParamPtr(appleEvent, kStatusCodeParam, typeInteger, &typeCode, &statusCode, sizeof(int), &actualSize);
	AEGetParamPtr(appleEvent, kErrorMsgParam, typeChar, &typeCode, cError, sizeof(cError), &actualSize);
	errorMsg.assign(cError, actualSize);

	AEGetParamPtr(appleEvent, kContentParam, typeChar, &typeCode, cContent, sizeof(cContent), &actualSize);
	content.assign(cContent, actualSize);

	MacWifiResponse response;

	response.Success = success;
	response.StatusCode = statusCode;
	response.ErrorMsg = errorMsg;
	response.Content = content;

	_callbacks[callbackId](response);
	_callbacks.erase(callbackId);
}