#include <stdio.h>
#include "WifiModule.h"

HttpClient _httpClient;

HttpClient& WifiModule::GetHttpClient()
{
	return _httpClient;
}

void WifiModule::DebugStr(std::string msg)
{
	FILE *fp;
	fp = fopen("Mac HD (68K):wifi.txt", "a");

	if (fp)
	{
		fprintf(fp, msg.c_str());
		fflush(fp);
	}

	fclose(fp);
}