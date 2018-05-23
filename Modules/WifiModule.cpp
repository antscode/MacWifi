#include <stdio.h>
#include <string.h>
#include "WifiModule.h"
#include "../Util.h"

void WifiModule::DoError(std::string errorMsg)
{
	memset(WifiDataPtr->ErrorMsg, 0, sizeof(WifiDataPtr->ErrorMsg));
	strcpy(WifiDataPtr->ErrorMsg, errorMsg.c_str());

	WifiDataPtr->Error = true;
	WifiDataPtr->Status = Idle;
	WifiDataPtr->UpdateUI = true;
}