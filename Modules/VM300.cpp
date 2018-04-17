#include <string>
#include <sstream> 
#include "VM300.h"

void VM300::GetNetworks()
{
	// First login
	Comms::Http.Post(
		_baseUri + "/goform/login",
		"username=admin&z999=z999&password=admin&Login=&platform=pc",
		std::bind(&VM300::LoginResponse, this, _1));
}

void VM300::LoginResponse(HttpResponse response)
{
	if (response.Success)
	{
		// Once logged in, get networks
		Comms::Http.Get(
			_baseUri + "/goform/get_web_hotspots_list",
			std::bind(&VM300::GetNetworksResponse, this, _1));
	}
	else
	{
		SaveError(response.ErrorMsg);
	}
}

void VM300::GetNetworksResponse(HttpResponse response)
{
	if (response.Success)
	{
		std::vector<Network> networks;
	
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

		SaveNetworks(networks);
	}
	else
	{
		SaveError(response.ErrorMsg);
	}
}