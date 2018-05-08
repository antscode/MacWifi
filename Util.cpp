#include <cctype>
#include <iomanip>
#include <sstream>
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

std::string Util::PtoStr(unsigned char *s)
{
	return std::string(PtoCStr(s));
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

void Util::FrameDefaultButton(DialogPtr dialog, short itemNo, bool active)
{
	DialogItemType type;
	ControlRef control;
	Handle itemH;
	Rect box;

	GetDialogItem(dialog, itemNo, &type, &itemH, &box);
	InsetRect(&box, -4, -4);
	PenSize(3, 3);

	if (!active)
	{
		RGBColor color;
		PixPatHandle pp;

		color.red = 0x8000;
		color.green = 0x8000;
		color.blue = 0x8000;

		pp = NewPixPat();
		MakeRGBPat(pp, &color);
		PenPixPat(pp);
		FrameRoundRect(&box, 16, 16);
		DisposePixPat(pp);
		PenNormal();
		HiliteControl((ControlRef)itemH, 255);
	}
	else
	{
		FrameRoundRect(&box, 16, 16);
		HiliteControl((ControlRef)itemH, 0);
	}
}

string Util::UrlEncode(string &value) 
{
	ostringstream escaped;
	escaped.fill('0');
	escaped << hex;

	for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
		string::value_type c = (*i);

		// Keep alphanumeric and other accepted characters intact
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			escaped << c;
			continue;
		}

		// Any other characters are percent-encoded
		escaped << uppercase;
		escaped << '%' << setw(2) << int((unsigned char)c);
		escaped << nouppercase;
	}

	return escaped.str();
}