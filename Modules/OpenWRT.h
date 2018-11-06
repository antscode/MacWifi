#ifndef _OPENWRT_
#define _OPENWRT_

#include <functional>
#include <json/json.h>
#include "WifiModule.h"
#include "../Util.h"

class OpenWRT : public WifiModule
{
public:
	virtual void GetNetworks();
	virtual void Connect(string name, string id, WifiMode mode, WifiEncryption encryption, string pwd);
	virtual void GetTunnel(string connect, function<void(GetTunnelResult)> onComplete);

private:
	std::function<void()> _onLoginComplete;
	std::function<void(string)> _onLoginError;
	string _ssid;
	string _bssid;
	WifiMode _mode;
	WifiEncryption _encryption;
	string _token;
	string _pwd;
	string _currentSsid;

	bool _tunnelsInited = false;
	map<string, int> _tunnels;
	string _tunnelId;
	string _tunnelConnect;
	int _tunnelPort;
	function<void(GetTunnelResult)> _onAddTunnelComplete = 0;

	void Login(std::function<void()> onComplete, std::function<void(string)> onError);
	void LoginResponse(HttpResponse response);

	void GetConnectedNetworkRequest();
	void GetConnectedNetworkResponse(HttpResponse response);

	void GetNetworksRequest();
	void GetNetworksResponse(HttpResponse response);

	void SetSsidRequest();
	void SetBssidRequest(HttpResponse response);
	void SetEncryptionRequest(HttpResponse response);
	void SetKeyRequest(HttpResponse response);
	void CommitRequest(HttpResponse response);
	void ReloadRequest(HttpResponse response);
	void ReloadResponse(HttpResponse response);

	void InitStunnel();
	void PopulateTunnelCache(HttpResponse response);
	void AddOrGetTunnel();
	void SetTunnelClient(HttpResponse response);
	void SetTunnelPort(HttpResponse response);
	void SetTunnelConnect(HttpResponse response);
	void CommitTunnel(HttpResponse response);
	void StunnelRestart(HttpResponse response);
	void AddTunnelToCache(HttpResponse response);
	void TunnelError(string errorMsg);

	WifiMode GetWifiMode(const Json::Value& mode);
	string GetEncryptionStr(WifiMode mode);
	WifiEncryption GetEncryption(const Json::Value& encryption);
};

#endif // _OPENWRT_
