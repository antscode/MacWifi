#ifndef _WIFI_MODULE_
#define _WIFI_MODULE_

#include <string>
#include <vector>
#include <functional>
#include <MacHTTP/HttpClient.h>
#include "../Comms.h"
#include "../WifiShared.h"

using namespace std;
using namespace std::placeholders;

class WifiModule
{
	public:
		WifiData* WifiDataPtr;
		virtual void GetNetworks() = 0;
		virtual void Connect(string name, string id, WifiMode mode, WifiEncryption encryption, string pwd) = 0;
		virtual void GetTunnel(string connect, function<void(string, int)> onComplete) = 0;

	protected:
		void DoError(string errorMsg);
};

#endif // _WIFI_MODULE_ 