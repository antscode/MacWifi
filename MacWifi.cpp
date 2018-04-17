#include <Events.h>
#include <AppleEvents.h>
#include "Modules\VM300.h"

void EventInit();
pascal OSErr Quit(AppleEvent appleEvent, AppleEvent reply, long refCon);
bool _run = true;

int main()
{	
	VM300 vm300;
	EventRecord event;
	bool getNetworks = true;
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
			if (getNetworks)
			{
				vm300.GetNetworks();
				getNetworks = false;
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
}

pascal OSErr Quit(AppleEvent appleEvent, AppleEvent reply, long refCon)
{
	_run = false;
}