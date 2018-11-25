#ifndef _PREFS_
#define _PREFS_

struct PrefData
{
	int Device;
	char Hostname[255];
	char Username[255];
	char Password[255];
};

class Prefs
{
	public:
		Prefs();
		PrefData Data;
		bool Save();
	private:
		bool GetPrefsSpec(FSSpec *theSpec);
};

#endif // _PREFS_ 