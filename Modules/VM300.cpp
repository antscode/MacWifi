#include <string>
#include "VM300.h"

void VM300::Login()
{
	GetHttpClient().Post(
		_baseUri + "/goform/login",
		"username=admin&z999=z999&password=admin&Login=&platform=pc",
		std::bind(&VM300::LoginResponse, this, _1));

	DebugStr(GetHttpClient().GetUri().Host);
}

void VM300::LoginResponse(HttpResponse response)
{
	DebugStr(response.ErrorMsg);
	//GetNetworks();
}

void VM300::GetNetworks()
{
	GetHttpClient().Get(
		_baseUri + "/goform/get_web_hotspots_list",
		std::bind(&VM300::GetNetworksResponse, this, _1));
}

void VM300::GetNetworksResponse(HttpResponse response)
{
	DebugStr("GetNetworks response: " + std::to_string(response.StatusCode));
}