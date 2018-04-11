#ifndef _WIFI_MODULE_
#define _WIFI_MODULE_

#include <string>
#include <vector>
#include <functional>
#include "../Network.h"

using namespace std::placeholders;

class WifiModule
{
	public:
		virtual void GetNetworks(std::function<void(std::vector<Network>)> onComplete);

	protected:
		std::function<void(std::vector<Network>)> GetNetworksComplete;
};

#endif // _WIFI_MODULE_ 