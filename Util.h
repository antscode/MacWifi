#include <string> 
#include <Dialogs.h>

class Util
{
	public:
		static char* PtoCStr(unsigned char *s);
		static unsigned char* CtoPStr(char *s);
		static unsigned char* StrToPStr(std::string str);
		static void Debug(std::string msg);
		static void FrameDefaultButton(DialogPtr dialog, short itemNo, bool active);
};