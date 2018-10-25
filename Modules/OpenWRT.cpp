#include <string>
#include <sstream> 
#include <algorithm>
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

void OpenWRT::GetTunnel(string connect, function<void(GetTunnelResult)> onComplete)
{
	transform(connect.begin(), connect.end(), connect.begin(), ::tolower);
	connect += ":443";
	_tunnelConnect = connect;
	_onAddTunnelComplete = onComplete;

	if (!_tunnelsInited)
	{
		Login(bind(&OpenWRT::InitStunnel, this));
	}
	else
	{
		Login(bind(&OpenWRT::AddOrGetTunnel, this));
	}
}

void OpenWRT::InitStunnel()
{
	_tunnels.clear();
	_tunnelPort = 2000;

	Comms::Http.Post(
		"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
		"{ \"id\": 1, \"method\": \"get_all\", \"params\": [ \"stunnel\" ] }",
		std::bind(&OpenWRT::PopulateTunnelCache, this, _1));
}

void OpenWRT::PopulateTunnelCache(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Json::Value root;
			Json::Reader reader;
			string connect;
			int port;
			bool parseSuccess = reader.parse(response.Content.c_str(), root);

			if (parseSuccess)
			{
				if (root.isMember("error") && !root["error"].empty())
				{
					TunnelError("PopulateTunnelCache: " + root["error"]["message"].asString());
					return;
				}

				const Json::Value& tunnels = root["result"];

				vector<string> members = tunnels.getMemberNames();

				for (string& member : members)
				{
					// Is this a valid tunnel?
					if (tunnels[member].isMember(".type") &&
						tunnels[member][".type"].asString() == "service" &&
						tunnels[member].isMember("accept_port") &&
						!tunnels[member]["accept_port"].empty() &&
						tunnels[member].isMember("connect") &&
						!tunnels[member]["connect"].empty() &&
						tunnels[member].isMember("client") &&
						tunnels[member]["client"].asString() == "yes")
					{
						connect = tunnels[member]["connect"].asString();
						transform(connect.begin(), connect.end(), connect.begin(), ::tolower);
						port = stoi(tunnels[member]["accept_port"].asString());

						if (_tunnels.count(connect) == 0)
						{
							_tunnels.insert(pair<string, int>(connect, port));

							if (port > _tunnelPort)
							{
								_tunnelPort = port;
							}
						}
					}
				}

				_tunnelsInited = true;
				AddOrGetTunnel();
			}
			else
			{
				TunnelError("PopulateTunnelCache: Error parsing response.");
			}
		}
		else
		{
			TunnelError("PopulateTunnelCache: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("PopulateTunnelCache: " + response.ErrorMsg);
	}
}

void OpenWRT::AddOrGetTunnel()
{
	if (_tunnels.count(_tunnelConnect) > 0)
	{
		// Already in cache, return port
		GetTunnelResult result;

		result.Success = true;
		result.Host = string(WifiDataPtr->Hostname);
		result.Port = _tunnels[_tunnelConnect];

		_onAddTunnelComplete(result);
	}
	else
	{
		// Not in cache, so add to stunnel
		Comms::Http.Post(
			"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
			"{ \"id\": 1, \"method\": \"add\", \"params\": [ \"stunnel\", \"service\" ] }",
			std::bind(&OpenWRT::SetTunnelClient, this, _1));
	}
}

void OpenWRT::SetTunnelClient(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Json::Value root;
			Json::Reader reader;
			string connect;
			bool parseSuccess = reader.parse(response.Content.c_str(), root);

			if (parseSuccess)
			{
				if (root.isMember("error") && !root["error"].empty())
				{
					TunnelError("SetTunnelClient: " + root["error"]["message"].asString());
					return;
				}

				_tunnelId = root["result"].asString();

				Comms::Http.Post(
					"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
					"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"stunnel\", \"" + _tunnelId + "\", \"client\", \"yes\" ] }",
					std::bind(&OpenWRT::SetTunnelPort, this, _1));

			}
			else
			{
				TunnelError("SetTunnelClient: Error parsing response.");
			}
		}
		else
		{
			TunnelError("SetTunnelClient: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("SetTunnelClient: " + response.ErrorMsg);
	}
}

void OpenWRT::SetTunnelPort(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			_tunnelPort++;

			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"stunnel\", \"" + _tunnelId + "\", \"accept_port\", \"" + to_string(_tunnelPort) + "\" ] }",
				std::bind(&OpenWRT::SetTunnelConnect, this, _1));

		}
		else
		{
			TunnelError("SetTunnelPort: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("SetTunnelPort: " + response.ErrorMsg);
	}
}

void OpenWRT::SetTunnelConnect(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"set\", \"params\": [ \"stunnel\", \"" + _tunnelId + "\", \"connect\", \"" + _tunnelConnect + "\" ] }",
				std::bind(&OpenWRT::CommitTunnel, this, _1));

		}
		else
		{
			TunnelError("SetTunnelConnect: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("SetTunnelConnect: " + response.ErrorMsg);
	}
}

void OpenWRT::CommitTunnel(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/uci?auth=" + _token,
				"{ \"id\": 1, \"method\": \"commit\", \"params\": [ \"stunnel\" ] }",
				std::bind(&OpenWRT::StunnelRestart, this, _1));
		}
		else
		{
			TunnelError("CommitTunnel: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("CommitTunnel: " + response.ErrorMsg);
	}
}

void OpenWRT::StunnelRestart(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			Comms::Http.Post(
				"http://" + string(WifiDataPtr->Hostname) + "/cgi-bin/luci/rpc/sys?auth=" + _token,
				"{ \"id\": 1, \"method\": \"exec\", \"params\": [ \"/etc/init.d/stunnel restart\" ] }",
				std::bind(&OpenWRT::AddTunnelToCache, this, _1));
		}
		else
		{
			TunnelError("StunnelRestart: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("StunnelRestart: " + response.ErrorMsg);
	}
}

void OpenWRT::AddTunnelToCache(HttpResponse response)
{
	if (response.Success)
	{
		if (response.StatusCode == 200)
		{
			if (_tunnels.count(_tunnelConnect) == 0)
			{
				_tunnels.insert(pair<string, int>(_tunnelConnect, _tunnelPort));

				GetTunnelResult result;

				result.Success = true;
				result.Host = string(WifiDataPtr->Hostname);
				result.Port = _tunnelPort;

				// Give stunnel time to restart before issuing requests (1 sec)
				Util::Sleep(1);

				_onAddTunnelComplete(result);
			}
		}
		else
		{
			TunnelError("AddTunnelToCache: " + to_string(response.StatusCode) + " status returned.");
		}
	}
	else
	{
		TunnelError("AddTunnelToCache: " + response.ErrorMsg);
	}
}

void OpenWRT::TunnelError(string errorMsg)
{
	GetTunnelResult result;

	result.Success = false;
	result.ErrorMsg = errorMsg;

	_onAddTunnelComplete(result);
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
