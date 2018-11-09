#include <AppleEvents.h>

// Event Classes
enum {
	kWifiClass = FOUR_CHAR_CODE('wifi')
};

// Event Id's
enum {
	kRequest = FOUR_CHAR_CODE('requ'), 
	kResponse = FOUR_CHAR_CODE('resp')
};

// Event Parameters
enum {
	kMethodParam = FOUR_CHAR_CODE('mthd'),
	kUriParam = FOUR_CHAR_CODE('ruri'),
	kDataParam = FOUR_CHAR_CODE('data'),
	kCallbackIdParam = FOUR_CHAR_CODE('cbck'),
	kSuccessParam = FOUR_CHAR_CODE('scss'),
	kStatusCodeParam = FOUR_CHAR_CODE('stat'),
	kErrorMsgParam = FOUR_CHAR_CODE('errm'),
	kContentParam = FOUR_CHAR_CODE('cont'),
	kAuthorizationParam = FOUR_CHAR_CODE('auth')
};