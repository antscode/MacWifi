#include <Dialogs.h>
#include "Modules\WifiModule.h"

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

bool _run = true;

int main();
void GetSharedData();
void EventInit();
void GetPrefs();
void GetWifiModule();
void Restart();
void SavePrefs();
void InitTunnel(string requestUri);

bool _requestComplete;
HttpResponse _response;
void RequestComplete(HttpResponse response);

pascal OSErr ProcessRequestEvent(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);