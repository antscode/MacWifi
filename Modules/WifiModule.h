#ifndef _WIFI_MODULE_
#define _WIFI_MODULE_

#include <string>
#include <vector>
#include <functional>
#include <MacHTTP/HttpClient.h>
#include "../Comms.h"
#include "../Network.h"

using namespace std::placeholders;

class WifiModule
{
	public:
		virtual void GetNetworks() = 0;

	protected:
		void SaveNetworks(std::vector<Network> networks);
		void SaveError(std::string errorMsg);
};

#endif // _WIFI_MODULE_ 