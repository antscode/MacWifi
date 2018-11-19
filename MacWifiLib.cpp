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
	const char* cAuthorization = _authorization.c_str();

	int callbackIndex = _callbacks.size();
	_callbacks.insert(pair<int, function<void(MacWifiResponse)>>(callbackIndex, onComplete));

	AEPutParamPtr(&appleEvent, kMethodParam, typeChar, cMethod, strlen(cMethod));
	AEPutParamPtr(&appleEvent, kUriParam, typeChar, cUri, strlen(cUri));
	AEPutParamPtr(&appleEvent, kDataParam, typeChar, cContent, strlen(cContent));
	AEPutParamPtr(&appleEvent, kAuthorizationParam, typeChar, cAuthorization, strlen(cAuthorization));
	AEPutParamPtr(&appleEvent, kUtf8ToMacRomanParam, typeBoolean, &_utf8ToMacRoman, sizeof(bool));
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

	_callbacks[callbackId](response);
	_callbacks[callbackId] = 0;
}

void MacWifiLib::GetParamAsString(AppleEvent* appleEvent, AEKeyword keyword, string &output)
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

string MacWifiLib::Encode(const string &value)
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