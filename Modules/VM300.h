#ifndef _VM300_
#define _VM300_

#include <functional>
#include "WifiModule.h"
#include "../Util.h"

class VM300 : public WifiModule
{
	public:
		virtual void GetNetworks();
		virtual void Connect(std::string ssid, WifiMode mode, WifiEncryption encryption, std::string pwd);
		
	private:
		const std::string _baseUri = "http://vonets.cfg";
		std::function<void()> _onLoginComplete;
		std::string _ssid;
		WifiMode _mode;
		WifiEncryption _encryption;
		std::string _pwd;

		void Login(std::function<void()> onComplete);
		void LoginResponse(HttpResponse response);
		
		void GetNetworksRequest();
		void GetNetworksResponse(HttpResponse response);

		void ConnectRequest();
		void ConnectResponse(HttpResponse response);

		void Restart();
		void RestartRequest();
		void RestartResponse(HttpResponse response);

		std::string GetWifiModeStr(WifiMode mode);
		std::string GetEncryptionStr(WifiEncryption encryption);
};

#endif // _VM300_