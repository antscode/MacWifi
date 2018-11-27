#ifndef __MACWIFI__
#define __MACWIFI__

#include <string>
#include <functional>
#include <map>
#include "WifiEvents.h"

using namespace std;

class MacWifiResponse
{
	public:
		bool Success;
		unsigned int StatusCode;
		string ErrorMsg;
		string Content;
};

class MacWifiLib
{
	public:
		MacWifiLib();
		void Get(const string& requestUri, function<void(MacWifiResponse&)> onComplete);
		void Post(const string& requestUri, const string& content, function<void(MacWifiResponse&)> onComplete);
		void Put(const string& requestUri, const string& content, function<void(MacWifiResponse&)> onComplete);
		void SetAuthorization(string authorization);
		OSErr ProcessReply(AppleEvent* appleEvent);
		static string Encode(const string &value);
		void Utf8ToMacRoman(bool enabled);

	private:
		OSType _appSig;
		string _authorization;
		bool _utf8ToMacRoman;
		map<int, function<void(MacWifiResponse&)>> _callbacks;
		void SendRequestEvent(const string& method, const string& uri, const string& content, function<void(MacWifiResponse&)> onComplete);
		void GetEventAddress(AEAddressDesc* address);
		OSErr SendEvent(AppleEvent* appleEvent);
		void GetParamAsString(AppleEvent* appleEvent, AEKeyword keyword, string& output);
};

#endif
