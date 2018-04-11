#include <string> 

class Util
{
	public:
		static char* PtoCStr(unsigned char *s);
		static unsigned char* CtoPStr(char *s);
		static unsigned char* StrToPStr(std::string str);
		static void Debug(std::string msg);
};