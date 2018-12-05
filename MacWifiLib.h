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
		long Id;
};

class MacWifiCallback
{
	public:
		function<void(MacWifiResponse&)> Callback;
		long Id;
};

class MacWifiLib
{
	public:
		MacWifiLib();
		void Get(const string& requestUri, function<void(MacWifiResponse&)> onComplete, long id = 0);
		void Post(const string& requestUri, const string& content, function<void(MacWifiResponse&)> onComplete, long id = 0);
		void Put(const string& requestUri, const string& content, function<void(MacWifiResponse&)> onComplete, long id = 0);
		void SetAuthorization(string authorization);
		OSErr ProcessReply(AppleEvent* appleEvent);
		static string Encode(const string &value);
		void Utf8ToMacRoman(bool enabled);

	private:
		OSType _appSig;
		string _authorization;
		bool _utf8ToMacRoman;
		map<int, MacWifiCallback> _callbacks;
		int _callbackIndex = 0;
		void SendRequestEvent(const string& method, const string& uri, const string& content, function<void(MacWifiResponse&)> onComplete, long id);
		void GetEventAddress(AEAddressDesc* address);
		OSErr SendEvent(AppleEvent* appleEvent);
		void GetParamAsString(AppleEvent* appleEvent, AEKeyword keyword, string& output);
};

#endif
