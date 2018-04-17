#include "Retro68.r"
#include "Finder.r"
#include "MacTypes.r"
#include "Processes.r"

type 'INIT' {
	RETRO68_CODE_TYPE
};

resource 'INIT' (128, sysheap, locked, nonpurgeable) {
	dontBreakAtEntry, $$read("MacWifiINIT");
};

resource 'FREF' (128, purgeable) {
   'appe', 0, ""
};

type 'MWFI' as 'STR ';
resource 'MWFI' (0, purgeable) {
   "MacWifi Extension 1.0"
};

resource 'BNDL' (128, purgeable) {
   'MWFI', 0,
   {  'ICN#', {0, 128},
      'FREF', {0, 128}
   }
};

resource 'SIZE' (-1) {
	dontSaveScreen,
	acceptSuspendResumeEvents,
	enableOptionSwitch,
	canBackground,
	multiFinderAware,
	onlyBackground,
	dontGetFrontClicks,
	ignoreChildDiedEvents,
	is32BitCompatible,
	isHighLevelEventAware,
	onlyLocalHLEvents,
	notStationeryAware,
	reserved,
	reserved,
	reserved,
	reserved,
	1000 * 1024,
	1000 * 1024
};

data 'ICN#' (128) {
	$"7FFF FFF0 8000 0008 8000 0008 8000 0008"            /* ....─...─...─... */
	$"8000 0008 8000 0008 8000 0008 8000 0008"            /* ─...─...─...─... */
	$"A000 0008 D000 000A 9000 000D 1007 E009"            /* ═...п...░..б...ф */
	$"101F F801 107C 3E01 10F0 0F01 1043 C201"            /* .....|>......Cб. */
	$"100F F001 101E 7801 1008 1001 1001 8001"            /* ......x.......─. */
	$"1003 C009 9001 800D D000 000A A000 0008"            /* ..юф░.─бп...═... */
	$"8000 0008 8000 0008 8000 0008 8000 0008"            /* ─...─...─...─... */
	$"8000 0008 8000 0008 8000 0008 7FFF FFF0"            /* ─...─...─....... */
	$"7FFF FFF0 FFFF FFF8 FFFF FFF8 FFFF FFF8"            /* ................ */
	$"FFFF FFF8 FFFF FFF8 FFFF FFF8 FFFF FFF8"            /* ................ */
	$"FFFF FFF8 DFFF FFFA 9FFF FFFF 1FFF FFFF"            /* ........÷....... */
	$"1FFF FFFF 1FFF FFFF 1FFF FFFF 1FFF FFFF"            /* ................ */
	$"1FFF FFFF 1FFF FFFF 1FFF FFFF 1FFF FFFF"            /* ................ */
	$"1FFF FFFF 9FFF FFFF DFFF FFFA FFFF FFF8"            /* ....÷........... */
	$"FFFF FFF8 FFFF FFF8 FFFF FFF8 FFFF FFF8"            /* ................ */
	$"FFFF FFF8 FFFF FFF8 FFFF FFF8 7FFF FFF0"            /* ................ */
};

data 'ICN#' (129) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
};

data 'ics#' (128) {
	$"FFFE 8002 8002 8002 83C2 CFF3 5C39 5BD9"            /* ..─.─.─.┐бо.\9[. */
	$"47E1 4661 C183 8182 8002 8002 8002 FFFE"            /* G.Faа┐│┌─.─.─... */
	$"FFFE FFFE FFFE FFFE FFFE FFFF 7FFF 7FFF"            /* ................ */
	$"7FFF 7FFF FFFF FFFE FFFE FFFE FFFE FFFE"            /* ................ */
};

data 'ics#' (129) {
	$"0000 0000 0000 07E0 1FF8 7C3E F00F 43C2"            /* ..........|>..Cб */
	$"0FF0 1E78 0810 0180 03C0 0180 0000 0000"            /* ...x...─.ю.─.... */
	$"0000 0000 0000 07E0 1FF8 7C3E F00F 43C2"            /* ..........|>..Cб */
	$"0FF0 1E78 0810 0180 03C0 0180 0000 0000"            /* ...x...─.ю.─.... */
};

data 'icl8' (128) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 0000 0000"            /* ................ */
	$"FF00 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 00F6 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F7F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 FFF7 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FFFF 00FF 00F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 FF00"            /* ................ */
	$"FF00 00FF 00F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FFFF 00FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F6 F6F6 F6FF FFFF"            /* ................ */
	$"FFFF FFF6 F6F6 F6F6 F6F6 F6F7 FF00 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F6 F6FF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFF6 F6F6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6FF FFFF FFFF F7F7"            /* ................ */
	$"F7F7 FFFF FFFF FFF6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 FFFF FFFF F7F7 F6F6"            /* ................ */
	$"F6F6 F7F7 FFFF FFFF F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F7FF F7F7 F6F6 FFFF"            /* ................ */
	$"FFFF F6F6 F7F7 FFF7 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F7 F6F6 FFFF FFFF"            /* ................ */
	$"FFFF FFFF F6F6 F7F6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F6 F6FF FFFF FFF7"            /* ................ */
	$"F7FF FFFF FFF6 F6F6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F6 F6F7 FFF7 F7F6"            /* ................ */
	$"F6F7 F7FF F7F6 F6F6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F6 F6F6 F7F6 F6FF"            /* ................ */
	$"FFF6 F6F7 F6F6 F6F6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"0000 00FF 00F6 F6F6 F6F6 F6F6 F6F6 FFFF"            /* ................ */
	$"FFFF F6F6 F6F6 F6F6 F6F6 F6F7 FFF7 F7FF"            /* ................ */
	$"FF00 00FF 00F6 F6F6 F6F6 F6F6 F6F6 F7FF"            /* ................ */
	$"FFF7 F6F6 F6F6 F6F6 F6F6 F6F7 FFFF F7FF"            /* ................ */
	$"FFFF 00FF 00F6 F6F6 F6F6 F6F6 F6F6 F6F7"            /* ................ */
	$"F7F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 FF00"            /* ................ */
	$"FF00 FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 00F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6"            /* ................ */
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00 0000"            /* ................ */
	$"FFF6 F7F7 F7F7 F7F7 F7F7 F7F7 F7F7 F7F7"            /* ................ */
	$"F7F7 F7F7 F7F7 F7F7 F7F7 F7F7 FF00 0000"            /* ................ */
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF 0000 0000"            /* ................ */
};

data 'ics8' (128) {
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00"            /* ................ */
	$"FF00 0000 0000 0000 0000 0000 0000 FF00"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00"            /* ................ */
	$"FF00 F6F6 F6F6 FFFF FFFF F6F6 F6F7 FF00"            /* ................ */
	$"FFFF 00F6 FFFF FFFF FFFF FFFF F6F7 FFFF"            /* ................ */
	$"00FF 00FF FFFF F6F6 F6F6 FFFF FFF6 F7FF"            /* ................ */
	$"00FF 00FF FFF6 FFFF FFFF F6FF FFF6 F7FF"            /* ................ */
	$"00FF 00F6 F6FF FFFF FFFF FFF6 F6F6 F7FF"            /* ................ */
	$"00FF 00F6 F6FF FFF6 F6FF FFF6 F6F6 F7FF"            /* ................ */
	$"FFFF 00F6 F6F6 F6FF FFF6 F6F6 F6F7 FFFF"            /* ................ */
	$"FF00 F6F6 F6F6 F6FF FFF6 F6F6 F6F7 FF00"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00"            /* ................ */
	$"FF00 F6F6 F6F6 F6F6 F6F6 F6F6 F6F7 FF00"            /* ................ */
	$"FFF6 F7F7 F7F7 F7F7 F7F7 F7F7 F7F7 FF00"            /* ................ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00"            /* ................ */
};

data 'ics4' (128) {
	$"0FFF FFFF FFFF FF00 F000 0000 0000 00F0"            /* ................ */
	$"F0CC CCCC CCCC CDF0 F0CC CCCC CCCC CDF0"            /* .лллллм..лллллм. */
	$"F0CC CCFF FFCC CDF0 0F0C FFFF FFFF CDFF"            /* .лл..лм.......м. */
	$"0F0F FFCC CCFF FCDF 0F0F FCFF FFCF FCDF"            /* ...лл........о.. */
	$"0F0C CFFF FFFC CCDF 0F0C CFFC CFFC CCDF"            /* ..о...л...о.о.л. */
	$"0F0C CCCF FCCC CDFF F0CC CCCF FCCC CDF0"            /* ..ло.лм..лло.лм. */
	$"F0CC CCCC CCCC CDF0 F0CC CCCC CCCC CDF0"            /* .лллллм..лллллм. */
	$"F0DD DDDD DDDD DDF0 0FFF FFFF FFFF FF00"            /* ................ */
};

data 'icl4' (128) {
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF 0000"            /* ................ */
	$"F000 0000 0000 0000 0000 0000 000C F000"            /* ................ */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0FC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* ..лллллллллллм.. */
	$"FF0F CCCC CCCC CCCC CCCC CCCC CCCD F0F0"            /* ..лллллллллллм.. */
	$"F00F 0CCC CCCC CCCC CCCC CCCC CCCD FFCF"            /* ...ллллллллллм.о */
	$"000F 0CCC CCCC CFFF FFFC CCCC CCCD FCDF"            /* ...ллло...лллм.. */
	$"000F 0CCC CCCF FFFF FFFF FCCC CCCC CCDF"            /* ...лло.....лллл. */
	$"000F 0CCC CFFF FFDD DDFF FFFC CCCC CCDF"            /* ...ло.......ллл. */
	$"000F 0CCC FFFF DDCC CCDD FFFF CCCC CCDF"            /* ...л...лл...ллл. */
	$"000F 0CCC DFDD CCFF FFCC DDFD CCCC CCDF"            /* ...л..л..л..ллл. */
	$"000F 0CCC CDCC FFFF FFFF CCDC CCCC CCDF"            /* ...лмл....л.ллл. */
	$"000F 0CCC CCCF FFFD DFFF FCCC CCCC CCDF"            /* ...лло.....лллл. */
	$"000F 0CCC CCCD FDDC CDDF DCCC CCCC CCDF"            /* ...ллм..м..лллл. */
	$"000F 0CCC CCCC DCCF FCCD CCCC CCCC DCDF"            /* ...ллл.о.млллл.. */
	$"000F 0CCC CCCC CCFF FFCC CCCC CCCD FDDF"            /* ...лллл..ллллм.. */
	$"F00F 0CCC CCCC CCDF FDCC CCCC CCCD FFDF"            /* ...лллл..ллллм.. */
	$"FF0F 0CCC CCCC CCCD DCCC CCCC CCCD F0F0"            /* ...ллллм.ллллм.. */
	$"F0F0 CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* ..лллллллллллм.. */
	$"F00C CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* ..лллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"F0CC CCCC CCCC CCCC CCCC CCCC CCCD F000"            /* .ллллллллллллм.. */
	$"FCDD DDDD DDDD DDDD DDDD DDDD DDDD F000"            /* ................ */
	$"0FFF FFFF FFFF FFFF FFFF FFFF FFFF 0000"            /* ................ */
};