#include "Wifi.h"
#include "Modules/VM300.h"

void GetNetworks()
{
	VM300 vm300;
	HttpClient& httpClient = vm300.GetHttpClient();

	vm300.Login();
	
	while (httpClient.GetStatus() != HttpClient::RequestStatus::Idle)
	{
		httpClient.ProcessRequests();
	}
}