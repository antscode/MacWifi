#include "Modules\VM300.h"

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

bool _run = true;

int main();
void GetSharedData();
void EventInit();
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr ConnectNetwork(AppleEvent* appleEvent, AppleEvent* reply, long refCon);