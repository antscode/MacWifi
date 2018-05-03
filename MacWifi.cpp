#include <Events.h>
#include "WifiEvents.h"
#include "Modules\VM300.h"

void EventInit();
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr GetNetworks(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr ConnectNetwork(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
bool _run = true;
bool _getNetworks = true;

int main()
{	
	VM300 vm300;
	EventRecord event;
	
	int sleep = 60;

	EventInit();

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
			if (_getNetworks)
			{
				vm300.GetNetworks();
				_getNetworks = false;
			}

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
	_getNetworks = true;
}

pascal OSErr ConnectNetwork(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	OSErr   err;
	char blah[100];
	Size actualSize;
	DescType typeCode;

	err = AEGetParamPtr(appleEvent, 'ssid', typeChar, &typeCode, (Ptr)blah, sizeof(blah), &actualSize);

	Util::Debug(std::string(blah));
}