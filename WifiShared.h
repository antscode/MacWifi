#include <string>
#include <vector>

#ifndef _WIFI_SHARED_
#define _WIFI_SHARED_

using namespace std;

enum WifiStatus
{
	Idle,
	ScanRequest,
	Scanning,
	ConnectRequest,
	Connecting,
	RestartRequired,
	RestartRequest,
	Restarting
};

enum WifiMode
{
	WPA2,
	WPA,
	Open
};

enum WifiEncryption
{
	AES,
	TKIP,
	None
};

struct Network
{
	public:
		string Name;
		string Id;
		WifiMode Mode;
		WifiEncryption Encryption;
		bool Connected;
};

class WifiData
{
	public:
		WifiStatus Status;
		bool UpdateUI;
		bool Error;
		char ErrorMsg[255];
		vector<Network> Networks;
		char ConnectName[255];
		char ConnectId[255];
		char ConnectPwd[255];
		WifiMode ConnectMode;
		WifiEncryption ConnectEncryption;
};

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

#endif // _WIFI_SHARED_ 