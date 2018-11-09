#include <Dialogs.h>
#include "Modules\WifiModule.h"

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

enum RequestStatus
{
	Init,
	Request,
	Processing,
	Complete
};

bool _run = true;

int main();
void GetSharedData();
void EventInit();
void GetPrefs();
void GetWifiModule();
void Restart();
void SavePrefs();
void InitTunnel(Uri uri);
void InitTunnelComplete(GetTunnelResult result);
void DoRequest(string method, Uri uri, string authorization, string data);
void RequestComplete(HttpResponse response);
void DoError(string errorMsg);
void GetParamAsString(AppleEvent* appleEvent, AEKeyword keyword, string &output);

RequestStatus _requestStatus;
HttpResponse _response;

pascal OSErr ProcessRequestEvent(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);