#ifndef _VM300_
#define _VM300_

#include <MacHTTP/HttpClient.h>
#include "WifiModule.h"
#include "../Util.h"

class VM300 : public WifiModule
{
	public:
		virtual void GetNetworks(std::function<void(std::vector<Network>)> onComplete);
		
	private:
		bool _loggedIn;
		const std::string _baseUri = "http://vonets.cfg";
		void Login();
		void LoginResponse(HttpResponse response);
		void GetNetworksResponse(HttpResponse response);
};

#endif // _VM300_