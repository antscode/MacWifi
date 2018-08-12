#include <string>
#include <sstream> 
#include "OpenWRT.h"

void OpenWRT::Login(std::function<void()> onComplete)
{
	_onLoginComplete = onComplete;

	Comms::Http.Post(
		"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/auth",
		"{ \"id\": 1, \"method\": \"login\", \"params\": [ \"" + string(WifiDataPtr->Username) + "\", \"" + string(WifiDataPtr->Password) + "\" ] }",
		std::bind(&OpenWRT::LoginResponse, this, _1));
}

void OpenWRT::LoginResponse(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Json::Value root;
			Json::Reader reader;
			bool parseSuccess = reader.parse(response.Content.c_str(), root);

			if (parseSuccess)
			{
				if (root.isMember("error") && !root["error"].empty())
				{
					DoError("Login: " + root["error"]["message"].asString());
					return;
				}

				if (root.isMember("result") && !root["result"].empty())
				{
					_token = root["result"].asString();
					_onLoginComplete();
				}
				else
				{
					DoError("Login: No token in response.");
				}
			}
			else
			{
				DoError("Login: Error parsing response.");
			}
		}
		else
		{
			DoError("Login: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("Login: " + response.ErrorMsg);
	}
}

void OpenWRT::GetNetworks()
{
	Login(std::bind(&OpenWRT::GetConnectedNetworkRequest, this));
}

void OpenWRT::GetConnectedNetworkRequest()
{
	Comms::Http.Post(
		"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
		"{ \"id\": 1, \"method\": \"get_all\", \"params\": [ \"wireless\", \"@wifi-iface[0]\" ] }",
		std::bind(&OpenWRT::GetConnectedNetworkResponse, this, _1));
}

void OpenWRT::GetConnectedNetworkResponse(HttpResponse response)
{
	_currentSsid = "";

	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Json::Value root;
			Json::Reader reader;
			bool parseSuccess = reader.parse(response.Content.c_str(), root);

			if (parseSuccess)
			{
				if (root.isMember("error") && !root["error"].empty())
				{
					DoError("GetConnectedNetwork: " + root["error"]["message"].asString());
					return;
				}

				const Json::Value& network = root["result"];

				if (network.isMember("bssid"))
				{
					_currentSsid = network["bssid"].asString();
				}
			}
			else
			{
				DoError("GetConnectedNetwork: Error parsing response.");
			}

			GetNetworksRequest();
		}
		else
		{
			DoError("GetConnectedNetwork: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("GetConnectedNetwork: " + response.ErrorMsg);
	}
}

void OpenWRT::GetNetworksRequest()
{
	Comms::Http.Post(
		"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/sys?auth=" + _token,
		"{ \"id\": 1, \"method\": \"wifi.getiwinfo\", \"params\": [ \"@wifi-device[0]\", \"scanlist\" ] }",
		std::bind(&OpenWRT::GetNetworksResponse, this, _1));
}

void OpenWRT::GetNetworksResponse(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			WifiDataPtr->Networks.clear();

			Json::Value root;
			Json::Reader reader;
			bool parseSuccess = reader.parse(response.Content.c_str(), root);

			if (parseSuccess)
			{
				const Json::Value& networks = root["result"];

				for (int i = 0; i < networks.size(); i++) {
					Network network;
					network.Name = networks[i]["ssid"].asString();
					network.Id = networks[i]["bssid"].asString();
					network.Encryption = GetEncryption(networks[i]["encryption"]);
					network.Mode = GetWifiMode(networks[i]["encryption"]);
					network.Connected = (network.Id == _currentSsid);
					WifiDataPtr->Networks.push_back(network);
				}

				WifiDataPtr->Status = Idle;
				WifiDataPtr->UpdateUI = true;
			}
			else
			{
				DoError("GetNetworks: Error parsing response.");
			}
		}
		else
		{
			DoError("GetNetworks: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("GetNetworks: " + response.ErrorMsg);
	}
}

void OpenWRT::Connect(string name, string id, WifiMode mode, WifiEncryption encryption, string pwd)
{
	_ssid = name;
	_bssid = id;
	_mode = mode;
	_encryption = encryption;
	_pwd = pwd;

	Login(std::bind(&OpenWRT::SetSsidRequest, this));
}

void OpenWRT::SetSsidRequest()
{
	Comms::Http.Post(
		"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
		"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"wireless\", \"@wifi-iface[0]\", \"ssid\", \"" + _ssid + "\" ] }",
		std::bind(&OpenWRT::SetBssidRequest, this, _1));
}

void OpenWRT::SetBssidRequest(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"wireless\", \"@wifi-iface[0]\", \"bssid\", \"" + _bssid + "\" ] }",
				std::bind(&OpenWRT::SetEncryptionRequest, this, _1));
		}
		else
		{
			DoError("SetSsidRequest: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("SetSsidRequest: " + response.ErrorMsg);
	}
}

void OpenWRT::SetEncryptionRequest(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"wireless\", \"@wifi-iface[0]\", \"encryption\", \"" + GetEncryptionStr(_mode) + "\" ] }",
				std::bind(&OpenWRT::SetKeyRequest, this, _1));
		}
		else
		{
			DoError("SetBssidRequest: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("SetBssidRequest: " + response.ErrorMsg);
	}
}

void OpenWRT::SetKeyRequest(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"wireless\", \"@wifi-iface[0]\", \"key\", \"" + _pwd + "\" ] }",
				std::bind(&OpenWRT::CommitRequest, this, _1));
		}
		else
		{
			DoError("SetEncryptionRequest: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("SetEncryptionRequest: " + response.ErrorMsg);
	}
}

void OpenWRT::CommitRequest(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"commit\", \"params\": [ \"wireless\" ] }",
				std::bind(&OpenWRT::ReloadRequest, this, _1));
		}
		else
		{
			DoError("SetKeyRequest: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("SetKeyRequest: " + response.ErrorMsg);
	}
}

void OpenWRT::ReloadRequest(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/sys?auth=" + _token,
				"{ \"id\": 1, \"method\": \"exec\", \"params\": [ \"luci-reload\" ] }",
				std::bind(&OpenWRT::ReloadResponse, this, _1));
		}
		else
		{
			DoError("CommitRequest: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("CommitRequest: " + response.ErrorMsg);
	}
}

void OpenWRT::ReloadResponse(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			for (std::vector<Network>::iterator it = WifiDataPtr->Networks.begin(); it != WifiDataPtr->Networks.end(); ++it)
			{
				it->Connected = (it->Id == _bssid);
			}

			WifiDataPtr->Status = Idle;
			WifiDataPtr->UpdateUI = true;
		}
		else
		{
			DoError("ReloadRequest: " + std::to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		DoError("ReloadRequest: " + response.ErrorMsg);
	}
}

WifiMode OpenWRT::GetWifiMode(const Json::Value& encryption)
{
	if (encryption["wpa"].asInt() == 2)
		return WPA2;

	if (encryption["wpa"].asInt() < 2)
		return WPA;

	return Open;
}

string OpenWRT::GetEncryptionStr(WifiMode mode)
{
	switch (mode)
	{
		case WPA2:
			return "psk2";

		case WPA:
			return "psk";

		default:
			return "";
	}
}

WifiEncryption OpenWRT::GetEncryption(const Json::Value& encryption)
{
	if (encryption["auth_suites"][0] == "PSK")
		return AES;

	return None;
}
