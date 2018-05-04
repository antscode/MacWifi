#include "Modules\VM300.h"

bool _run = true;
VM300 _wifiModule;

int main();
void EventInit();
pascal OSErr Quit(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr GetNetworks(AppleEvent* appleEvent, AppleEvent* reply, long refCon);
pascal OSErr ConnectNetwork(AppleEvent* appleEvent, AppleEvent* reply, long refCon);