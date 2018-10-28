#include <stdio.h>
#include <string.h>
#include "MacWifiLib.h"
#include "Util.h"

#define arraylen(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

string _requests[4][2] = 
{
	{ "Small http request", "http://httpbin.org/status/418" },
	{ "Small https request", "https://httpbin.org/status/418" },
	{ "Facebook", "https://facebook.com" },
	{ "Google", "https://google.com" }
};

bool _doRequest = true;
int _curRequest = 0;
MacWifiLib _wifiLib;
Util _util;

void DoRequest(string title, string url);
void OnResponse(MacWifiResponse response);
pascal OSErr ProcessResponseEvent(AppleEvent* appleEvent, AppleEvent* reply, long refCon);

int main()
{
	EventRecord event;

	AEInstallEventHandler(
		kCoreEventClass,
		kAEAnswer,
		(AEEventHandlerUPP)ProcessResponseEvent,
		0L,
		false);

	while (_curRequest < arraylen(_requests))
	{
		if (_doRequest)
		{
			DoRequest(_requests[_curRequest][0], _requests[_curRequest][1]);
		}

		if (WaitNextEvent(everyEvent, &event, 0, NULL))
		{
			switch (event.what)
			{
				case kHighLevelEvent:
					AEProcessAppleEvent(&event);
					break;
			}
		}
	}

	printf("All done!\n");
	getchar(); getchar();
	return 0;
}

void DoRequest(string title, string url)
{
	printf("%s (press return)...\n", title.c_str());
	fflush(stdout);
	getchar(); getchar();

	printf("%s says:\n\n", url.c_str());

	_util.StartTimer();
	_wifiLib.Get(url, OnResponse);
	_doRequest = false; 
}

void OnResponse(MacWifiResponse response)
{
	int timeTaken = _util.StopTimer();
	printf("\n\nRequest took %dms.\n\n", timeTaken);

	printf("Status: %d\n\n", response.StatusCode);

	if (response.Success)
	{
		printf("\n\nContent:\n\n");

		printf("%s\n\n", response.Content.c_str());
	}
	else
	{
		printf("ERROR: %s\n\n", response.ErrorMsg.c_str());
	}

	_curRequest++;
	_doRequest = true;
}

pascal OSErr ProcessResponseEvent(AppleEvent* appleEvent, AppleEvent* reply, long refCon)
{
	_wifiLib.ProcessReply(appleEvent);
}