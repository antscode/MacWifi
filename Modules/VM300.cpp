#include <string>
#include <sstream> 
#include "VM300.h"

void VM300::Login()
{
	if (!_loggedIn)
	{
		HttpClient _httpClient;

		_httpClient.Post(
			_baseUri + "/goform/login",
			"username=admin&z999=z999&password=admin&Login=&platform=pc",
			std::bind(&VM300::LoginResponse, this, _1));

		while (!_loggedIn)
		{
			_httpClient.ProcessRequests();
		}
	}
}

void VM300::LoginResponse(HttpResponse response)
{
	_loggedIn = true;
}

void VM300::GetNetworks(std::function<void(std::vector<Network>)> onComplete)
{
	WifiModule::GetNetworks(onComplete);

	Login();

	HttpClient _httpClient;

	_httpClient.Get(
		_baseUri + "/goform/get_web_hotspots_list",
		std::bind(&VM300::GetNetworksResponse, this, _1));

	while (_httpClient.GetStatus() != HttpClient::RequestStatus::Idle)
	{
		_httpClient.ProcessRequests();
	}
}

void VM300::GetNetworksResponse(HttpResponse response)
{
	std::vector<Network> networks;

	// TODO: Any level of error handling!
	std::stringstream ss;
	std::string line;

	ss.str(response.Content);

	while (std::getline(ss, line))
	{
		std::stringstream linestream(line);
		std::string name;
		std::string id;

		std::getline(linestream, name, '\t'); 
		linestream >> id;	
		
		bool exists = false;
		for (std::vector<Network>::iterator it = networks.begin(); it != networks.end(); ++it)
		{
			if (it->Name == name) 
			{
				exists = true;
				break;
			}
		}

		if (!exists && name != "[HiddenSSID]")
		{
			Network network;
			network.Name = name;

			networks.push_back(network);
		}
	}

	GetNetworksComplete(networks);
}