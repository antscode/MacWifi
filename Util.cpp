#include <string.h>
#include <stdio.h>
#include "Util.h"

char* Util::PtoCStr(unsigned char *s)
{
	int theLen;
	int t;

	theLen = s[0];

	for (t = 0; t<theLen; t++)
		s[t] = s[t + 1];

	s[theLen] = '\0';

	return (char *)s;
}

unsigned char* Util::CtoPStr(char *s)
{
	int theLen;
	int t;

	theLen = strlen(s);

	for (t = theLen; t >= 1; t--)
		s[t] = s[t - 1];

	s[0] = (char)theLen;

	return (unsigned char *)s;
}

unsigned char* Util::StrToPStr(std::string str)
{
	char* s = (char*)str.c_str();

	int theLen;
	int t;

	theLen = strlen(s);

	for (t = theLen; t >= 1; t--)
		s[t] = s[t - 1];

	s[0] = (char)theLen;

	return (unsigned char *)s;
}

void Util::Debug(std::string msg)
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