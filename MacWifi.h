#include "Modules\VM300.h"

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

bool _run = true;

int main();
void GetSharedData();
void EventInit();
void Restart();
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);