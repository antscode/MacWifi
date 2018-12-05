#include <string.h>
#include <cctype>
#include <iomanip>
#include <sstream>
#include "MacWifiLib.h"
#include "Util.h"

MacWifiLib::MacWifiLib()
{
	_authorization = "";
	_utf8ToMacRoman = true;
}

void MacWifiLib::Get(const string& requestUri, function<void(MacWifiResponse&)> onComplete, long id)
{
	SendRequestEvent("GET", requestUri, "", onComplete, id);
}

void MacWifiLib::Post(const string& requestUri, const string& content, function<void(MacWifiResponse&)> onComplete, long id)
{
	SendRequestEvent("POST", requestUri, content, onComplete, id);
}

void MacWifiLib::Put(const string& requestUri, const string& content, function<void(MacWifiResponse&)> onComplete, long id)
{
	SendRequestEvent("PUT", requestUri, content, onComplete, id);
}

void MacWifiLib::SendRequestEvent(const string& method, const string& uri, const string& content, function<void(MacWifiResponse&)> onComplete, long id)
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
	const char* cAuthorization = _authorization.c_str();

	_callbackIndex++;
	MacWifiCallback callback;
	callback.Callback = onComplete;
	callback.Id = id;

	_callbacks.insert(pair<int, MacWifiCallback>(_callbackIndex, callback));

	AEPutParamPtr(&appleEvent, kMethodParam, typeChar, cMethod, strlen(cMethod));
	AEPutParamPtr(&appleEvent, kUriParam, typeChar, cUri, strlen(cUri));
	AEPutParamPtr(&appleEvent, kDataParam, typeChar, cContent, strlen(cContent));
	AEPutParamPtr(&appleEvent, kAuthorizationParam, typeChar, cAuthorization, strlen(cAuthorization));
	AEPutParamPtr(&appleEvent, kUtf8ToMacRomanParam, typeBoolean, &_utf8ToMacRoman, sizeof(bool));
	AEPutParamPtr(&appleEvent, kCallbackIdParam, typeInteger, &_callbackIndex, sizeof(int));

	OSErr err = SendEvent(&appleEvent);

	if (err != noErr)
	{
		MacWifiResponse response;
		response.Success = false;
		response.ErrorMsg = "Cound not send event due to error " + to_string(err);
		onComplete(response);
	}
}

void MacWifiLib::SetAuthorization(string authorization)
{
	_authorization = authorization;
}

void MacWifiLib::Utf8ToMacRoman(bool enabled)
{
	_utf8ToMacRoman = enabled;
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
	MacWifiResponse response;
	int callbackId;

	AEGetParamPtr(appleEvent, kCallbackIdParam, typeInteger, &typeCode, &callbackId, sizeof(int), &actualSize);
	AEGetParamPtr(appleEvent, kSuccessParam, typeBoolean, &typeCode, &response.Success, sizeof(bool), &actualSize);
	AEGetParamPtr(appleEvent, kStatusCodeParam, typeInteger, &typeCode, &response.StatusCode, sizeof(int), &actualSize);
	GetParamAsString(appleEvent, kErrorMsgParam, response.ErrorMsg);
	GetParamAsString(appleEvent, kContentParam, response.Content);
	response.Id = _callbacks[callbackId].Id;

	_callbacks[callbackId].Callback(response);
	_callbacks.erase(callbackId);
}

void MacWifiLib::GetParamAsString(AppleEvent* appleEvent, AEKeyword keyword, string& output)
{
	DescType typeCode;
	Size sizeOfParam, actualSize;
	char* buffer;
	string retVal;

	AESizeOfParam(appleEvent, keyword, &typeCode, &sizeOfParam);
	buffer = (char*)NewPtr(sizeOfParam + 2);

	AEGetParamPtr(appleEvent, keyword, typeChar, &typeCode, buffer, sizeOfParam + 1, &actualSize);
	buffer[actualSize] = '\0';

	output.assign(buffer, actualSize);
	DisposePtr(buffer);
}

string MacWifiLib::Encode(const string& value)
{
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		escaped << nouppercase;
	}

	return escaped.str();
}