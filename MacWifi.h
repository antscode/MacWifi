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
WifiModule* GetWifiModule();
void Restart();
void SavePrefs();
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);