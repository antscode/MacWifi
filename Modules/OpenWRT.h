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

private:
	const string _baseUri = "http://192.168.8.1";
	std::function<void()> _onLoginComplete;
	string _ssid;
	string _bssid;
	WifiMode _mode;
	WifiEncryption _encryption;
	string _token;
	string _pwd;
	string _currentSsid;

	void Login(std::function<void()> onComplete);
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

	WifiMode GetWifiMode(const Json::Value& mode);

	string GetEncryptionStr(WifiMode mode);
	WifiEncryption GetEncryption(const Json::Value& encryption);
};

#endif // _OPENWRT_
