#include "Modules\VM300.h"
#include "Modules\OpenWRT.h"

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

bool _run = true;

int main();
void GetSharedData();
void EventInit();
void Restart();
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);