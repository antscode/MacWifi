#ifndef _VM300_
#define _VM300_

#include "WifiModule.h"

class VM300 : public WifiModule
{
	private:
		const std::string _baseUri = "http://vonets.cfg";

	public:
		virtual void Login();
		virtual void LoginResponse(HttpResponse response);
		virtual void GetNetworks();
		virtual void GetNetworksResponse(HttpResponse response);
};

#endif // _VM300_