# MacWifi
MacWifi is a System Extension to add wifi functionality to your 68k Mac. MacWifi allows you to view available wifi 
networks and connect to a wifi network through a familiar menu in the menu bar. WPA2, WPA and public networks are supported.

## System Requirements
* MacWifi has been tested on System 7.5, however it should work from System 7.
* MacTCP or Open Transport installed

Your Mac will also need an ethernet device and a wifi device. Currently, MacWifi supports the Vonets VM300 wifi module. 
[Find out more about setting up wifi for your Mac](https://68kmla.org/forums/index.php?/topic/31078-adding-wi-fi-to-my-mac-se30/).

## About the Vonets VM300
The VM300 is one of the few modern wifi modules that will communicate with your vintage Mac ethernet card (specifically, 
it allows you to manually select 10mbs Half Duplex and Full Duplex communication). That said, the VM300 is a very basic 
device and has a few issues and limitations:

* When connecting to a wifi network, don’t enter the wrong password! Otherwise you’ll brick the VM300 and you’ll need to do a factory reset (by shorting pins 5 and 8 on the board)
* Connecting to a new wifi network requires the VM300 to restart - the process takes about 2 minutes.
* Fetching the network list takes about 10 seconds.
* If you issue too many requests (i.e. get the list of wifi networks too many times), the device appears to run out of memory and locks up – requiring a restart.
* Occasionally, the device fails to start – a power cycle fixes this.

## Building from Source
MacWifi requires Retro68 for compilation, and the following libraries:

* [MacTCPHelper](https://github.com/antscode/MacTCPHelper)
* [MacHTTP](https://github.com/antscode/MacHTTP)

First build and install the above libraries. Note: When building MacHTTP, set HTTPS_ENABLED to OFF in CMakeLists.txt. 
HTTPS is not required, and will cause linker errors if it's not disabled.

Once the above libraries are built and installed, execute these commands from the top level of the MacWifi directory:

    cd ..
    mkdir MacWifi-build
    cd MacWifi-build
    cmake ../MacWifi -DCMAKE_TOOLCHAIN_FILE=<<YOUR_PATH_TO_Retro68-build>>/toolchain/m68k-apple-macos/cmake/retro68.toolchain.cmake
    make

The MacWifi extension will be in the MacWifi-build directory as a bin file and disk image.

## Credits
* MacWifi is built using [Retro68](https://github.com/autc04/Retro68).
* System Menu INIT code based on "SysMenu" shell project by Matt Slot, circa 1995.
* Password field based on code snippet by Phil Kearney, circa 1994.
* Startup icon code by Peter Lewis, Jim Walker & François Pottier, 1994.