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