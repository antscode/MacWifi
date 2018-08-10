#ifndef _VM300_
#define _VM300_

#include <functional>
#include "WifiModule.h"
#include "../Util.h"

class VM300 : public WifiModule
{
	public:
		virtual void GetNetworks();
		virtual void Connect(string name, string id, WifiMode mode, WifiEncryption encryption, string pwd);
		
	private:
		std::function<void()> _onLoginComplete;
		string _ssid;
		WifiMode _mode;
		WifiEncryption _encryption;
		string _pwd;
		string _currentSsid;

		void Login(std::function<void()> onComplete);
		void LoginResponse(HttpResponse response);
		
		void GetConnectedNetworkRequest();
		void GetConnectedNetworkResponse(HttpResponse response);

		void GetNetworksRequest();
		void GetNetworksResponse(HttpResponse response);

		void DeleteHotspotsRequest();
		void DeleteHotspotsResponse(HttpResponse response);

		void ConnectRequest();
		void ConnectResponse(HttpResponse response);

		void Restart();
		void RestartRequest();
		void RestartResponse(HttpResponse response);

		string GetWifiModeStr(WifiMode mode);
		WifiMode GetWifiMode(string mode);

		string GetEncryptionStr(WifiEncryption encryption);
		WifiEncryption GetEncryption(string encryption);
};

#endif // _VM300_