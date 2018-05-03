#include <string>
#include <sstream> 
#include "VM300.h"

void VM300::Login(std::function<void()> onComplete)
{
	_onLoginComplete = onComplete;

	Comms::Http.Post(
		_baseUri + "/goform/login",
		"username=admin&z999=z999&password=admin&Login=&platform=pc",
		std::bind(&VM300::LoginResponse, this, _1));
}

void VM300::LoginResponse(HttpResponse response)
{
	if (response.Success)
	{
		_onLoginComplete();
	}
	else
	{
		SaveError(response.ErrorMsg);
	}
}

void VM300::GetNetworks()
{
	Login(std::bind(&VM300::GetNetworksRequest, this));
}

void VM300::GetNetworksRequest()
{
	Comms::Http.Get(
		_baseUri + "/goform/get_web_hotspots_list",
		std::bind(&VM300::GetNetworksResponse, this, _1));
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

void VM300::Connect(std::string ssid, WifiMode mode, WifiEncryption encryption, std::string pwd)
{
	_ssid = ssid;
	_mode = mode;
	_encryption = encryption;
	_pwd = pwd;

	Login(std::bind(&VM300::ConnectRequest, this));
}

void VM300::ConnectRequest()
{
	Comms::Http.Post(
		_baseUri + "/goform/wirelessBrdgApcli",
		"apcli_ssid=" + _ssid +
		"&apcli_mode=" + GetWifiModeStr(_mode) +
		"&apcli_enc=" + GetEncryptionStr(_encryption) +
		"&apcli_ishide=0"
		"&apcli_wpapsk=" + _pwd +
		"&apcli_issyn=1"
		"&apcli_repeaterssid=" + _ssid + "_64"
		"&ra_off=0"
		"&EnDynamicMatchPara=1"
		"&isDnsNeedChange=1"
		"&allow_motion_dect=0"
		"&dhcpEnableButton=0"
		"&ApcliMatchMode=2"
		"&ApcliBlkCount=0",
		std::bind(&VM300::ConnectResponse, this, _1));
}

void VM300::ConnectResponse(HttpResponse response)
{
	if (response.Success)
	{
		Restart();
	}
	else
	{
		SaveError(response.ErrorMsg);
	}
}

void VM300::Restart()
{
	Login(std::bind(&VM300::RestartRequest, this));
}

void VM300::RestartRequest()
{
	Comms::Http.Post(
		_baseUri + "/goform/SystemCommand",
		"command=reboot&SystemCommandSubmit=Restart",
		std::bind(&VM300::RestartResponse, this, _1));
}

void VM300::RestartResponse(HttpResponse response)
{
	// Do nothing I guess...
}

std::string VM300::GetWifiModeStr(WifiMode mode)
{
	switch (mode)
	{
		case WPA2PSK:
			return "WPA2PSK";

		default:
			return "";
	}
}

std::string VM300::GetEncryptionStr(WifiEncryption encryption) 
{
	switch (encryption)
	{
		case AES:
			return "AES";

		default:
			return "";
	}
}