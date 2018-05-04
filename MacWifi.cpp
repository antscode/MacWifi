#include <Events.h>
#include <string.h>
#include "WifiEvents.h"
#include "MacWifi.h"

int main()
{	
	EventRecord event;
	
	int sleep = 60;

	EventInit();
	_wifiModule.GetNetworks();

	while (_run)
	{
		if (WaitNextEvent(everyEvent, &event, sleep, NULL))
		{
			switch (event.what)
			{
				case kHighLevelEvent:
					AEProcessAppleEvent(&event);
					break;
			}
		}
		else
		{
			Comms::Http.ProcessRequests();
		}
	}
}

void EventInit()
{
	AEInstallEventHandler(
		kCoreEventClass,
		kAEQuitApplication,
		(AEEventHandlerUPP)Quit,
		0L,
		false);

	AEInstallEventHandler(
		kWifiClass,
		kGetNetworks,
		(AEEventHandlerUPP)GetNetworks,
		0L,
		false);

	AEInstallEventHandler(
		kWifiClass,
		kConnectNetwork,
		(AEEventHandlerUPP)ConnectNetwork,
		0L,
		false);
}

pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_run = false;
}

pascal OSErr GetNetworks(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_wifiModule.GetNetworks();
}

pascal OSErr ConnectNetwork(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	Size actualSize;
	DescType typeCode;
	char ssid[255];
	char pwd[255];

	memset(&ssid, 0, sizeof(ssid));
	memset(&pwd, 0, sizeof(pwd));

	AEGetParamPtr(appleEvent, kSSIDParam, typeChar, &typeCode, ssid, sizeof(ssid), &actualSize);
	AEGetParamPtr(appleEvent, kPasswordParam, typeChar, &typeCode, pwd, sizeof(ssid), &actualSize);

	_wifiModule.Connect(std::string(ssid), WPA2PSK, AES, std::string(pwd));
}