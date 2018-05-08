#include <stdio.h>
#include "WifiModule.h"
#include "../Util.h"

void WifiModule::DoError(std::string errorMsg)
{
	WifiDataPtr->Error = true;
	WifiDataPtr->ErrorMsg = errorMsg;
	WifiDataPtr->Networks.clear();
	WifiDataPtr->Status = Idle;
	WifiDataPtr->UpdateUI = true;
}