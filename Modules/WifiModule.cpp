#include <stdio.h>
#include "WifiModule.h"
#include "../Prefs.h"

Prefs _prefs;

void WifiModule::SaveNetworks(std::vector<Network> networks)
{
	// Remove existing networks
	_prefs.Data["networks"].clear();

	for (std::vector<Network>::iterator it = networks.begin(); it != networks.end(); ++it)
	{
		Json::Value network;

		network["name"] = it->Name;
		network["connected"] = it->Connected;

		_prefs.Data["networks"].append(network);
	}

	_prefs.Save();
}

void WifiModule::SaveError(std::string errorMsg)
{
	_prefs.Data["error"] = errorMsg;
	_prefs.Save();
}