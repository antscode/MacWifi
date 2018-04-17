#ifndef _VM300_
#define _VM300_

#include "WifiModule.h"
#include "../Util.h"

class VM300 : public WifiModule
{
	public:
		virtual void GetNetworks();
		
	private:
		const std::string _baseUri = "http://vonets.cfg";
		void LoginResponse(HttpResponse response);
		void GetNetworksResponse(HttpResponse response);
};

#endif // _VM300_