#include <stdio.h>
#include "WifiModule.h"

void WifiModule::GetNetworks(std::function<void(std::vector<Network>)> onComplete)
{
	GetNetworksComplete = onComplete;
}