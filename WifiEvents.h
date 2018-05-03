#include <AppleEvents.h>

// Event Classes
enum {
	kWifiClass = FOUR_CHAR_CODE('wifi')
};

// Event Id's
enum {
	kGetNetworks = FOUR_CHAR_CODE('getn'),
	kConnectNetwork = FOUR_CHAR_CODE('conn')
};

// Event Parameters
enum {
	kSSIDParam = FOUR_CHAR_CODE('ssid'),
	kPasswordParam = FOUR_CHAR_CODE('pswd'),
};