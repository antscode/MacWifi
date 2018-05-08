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
	Connecting
};

struct Network
{
	public:
		string Name;
		bool Connected;
};

class WifiData
{
	public:
		WifiStatus Status;
		bool UpdateUI;
		bool Error;
		string ErrorMsg;
		vector<Network> Networks;
		string ConnectSSID;
		string ConnectPwd;
};

typedef int MemLoc;
typedef MemLoc* MemLocPtr;
typedef MemLocPtr* MemLocHandle;

#endif // _WIFI_SHARED_ 