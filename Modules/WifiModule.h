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
		virtual void Connect(std::string name, std::string id, WifiMode mode, WifiEncryption encryption, std::string pwd) = 0;

	protected:
		void DoError(std::string errorMsg);
};

#endif // _WIFI_MODULE_ 