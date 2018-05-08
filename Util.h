#include <string> 
#include <Dialogs.h>

using namespace std;

class Util
{
	public:
		static char* PtoCStr(unsigned char *s);
		static std::string PtoStr(unsigned char *s);
		static unsigned char* CtoPStr(char *s);
		static unsigned char* StrToPStr(string str);
		static void Debug(string msg);
		static void FrameDefaultButton(DialogPtr dialog, short itemNo, bool active);
		static string UrlEncode(string &value);
};