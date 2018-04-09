#ifndef _WIFI_MODULE_
#define _WIFI_MODULE_

#include <string>
#include <MacHTTP/HttpClient.h>

using namespace std::placeholders;

class WifiModule
{
	public:
		HttpClient& GetHttpClient();
		virtual void Login() = 0;
		virtual void LoginResponse(HttpResponse response) = 0;
		virtual void GetNetworks() = 0;
		virtual void GetNetworksResponse(HttpResponse response) = 0;
		void DebugStr(std::string msg);
};

#endif // _WIFI_MODULE_ 