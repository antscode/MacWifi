# MacWifi
MacWifi is a System Extension to add wifi functionality to your 68k Mac. MacWifi allows you to view available wifi 
networks and connect to a wifi network through a familiar menu in the menu bar. WPA2, WPA and public networks are supported.

## System Requirements
* System 7 or greater. The [Thread Manager](http://macintoshgarden.org/apps/thread-manager) extension is required for OS versions below System 7.5
* MacTCP or Open Transport installed

Your Mac will also need an ethernet device and a wifi device. Currently, MacWifi supports the following wifi devices:

* OpenWRT devices (MacWifi has been tested on a [GL.iNet GL-AR300M16-Ext](https://www.gl-inet.com/products/gl-ar300m/), but other OpenWRT devices should work)
* [Vonets VM300](http://www.vonets.com/ProductViews.asp?D_ID=33) wifi module

The Vonets VM300 is less expensive than OpenWRT devices, however OpenWRT is recommended for better speed, stability and features. 

## About OpenWRT
OpenWRT is a Linux OS designed for network devices. Your device must have OpenWRT version 18.06.0 or later installed. You can download the latest release here:

[https://downloads.openwrt.org/](https://downloads.openwrt.org/) ([instructions](https://wiki.openwrt.org/doc/howto/generic.sysupgrade))

Note that if you plan to purchase a GL-AR300M device, not all models are supported. Only the models with NOR memory are supported - models with NAND memory will 
not work, as they cannot be upgraded to the latest version of OpenWRT. The following models are supported:

* GL-AR300M16﻿﻿﻿ (internal antennas, 16m NOR memory only)
* GL-AR300M16-Ext﻿ (external antennas, 16m NOR memory only)

For the AR300M, you'll find the correct upgrade package under ar71xx > generic > ar300m file ending in sysupgrade.bin. The quick way of updating firmware
is via the Luci interface under System > Backup / Flash Firmware. You can upload you package file from there.

You must also install the following packages:

* Json RPC ([instructions](https://github.com/openwrt/luci/wiki/JsonRpcHowTo))
* ethtool (via the Software page in the Luci interface)
* stunnel (as above). This package is only required if you want to use an SSL-enabled app that requires MacWifi.

Finally, for OpenWRT to be able to communicate with your vintage Mac ethernet card; you need to set the LAN port to operate at 10mbs Half Duplex 
(some Mac ethernet cards support full duplex, in which case replace the word _half_ with _full_ below).

Add the following to your Startup script in OpenWRT (System > Startup in Luci):

    ethtool -s eth1 autoneg off speed 10 duplex half

(where _eth1_ is the name of your LAN port that's connected to your Mac)

Note: For some reason, running ethtool on the AR300M does not work for eth1 - however it does work for eth0. After you've set eth0 to the correct speed
& duplex, you can then swap the LAN & WAN ports via the Luci interface. Go to Network > Interfaces in Luci and edit the LAN & WAN ports. Under 
_Physical Settings_ swap the eth1/eth0 ports for each interface.

### stunnel Configuration
The following addtional steps are only required if you want to use an SSL-enabled app that requires MacWifi. The stunnel configuration needs
a bit of tweaking to work with MacWifi.

SSH into your OpenWRT router, and edit the following files using vim:

    vim /etc/config/stunnel

	Remove all contents from this file (it should contain a globals section and dummy service).

Crash course in vim: Press *i* to enter insert mode, make your edits, press Escape to leave insert mode, then type *:wq* to save & exit.

    vim /etc/init.d/stunnel

	Line 96: Change from:
	    printf "accept = %s:%s\n" "$accept_host" "$accept_port" >> "$CONF_FILE"
	to:
	    printf "accept = %s\n" "$accept_port" >> "$CONF_FILE" 

	Line 134: Remove this line:
	    print_list_colon "$cfg" ciphers

## About the Vonets VM300
The VM300 was the first device I discovered that could communicate with a vintage Mac over 10mbs Half Duplex. Setup instructions are on
the [68KMLA Forums](https://68kmla.org/forums/index.php?app=forums&module=forums&controller=topic&id=31078).

The VM300 is a very basic device and has a few issues and limitations:

* When connecting to a wifi network, don’t enter the wrong password! Otherwise you’ll brick the VM300 and you’ll need to do a factory reset (by shorting pins 5 and 8 on the board)
* Connecting to a new wifi network requires the VM300 to restart - the process takes about 2 minutes.
* Fetching the network list takes about 10 seconds.
* If you issue too many requests (i.e. get the list of wifi networks too many times), the device appears to run out of memory and locks up – requiring a restart.
* Occasionally, the device fails to start – a power cycle fixes this.

## Building from Source
MacWifi requires Retro68 for compilation, and the following libraries:

* [MacTCPHelper](https://github.com/antscode/MacTCPHelper)
* [MacHTTP](https://github.com/antscode/MacHTTP)
* [gason++](https://github.com/antscode/gason--)
* [libiconv](https://github.com/LuaDist/libiconv)

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
* [iconvpp](https://github.com/unnonouno/iconvpp) C++ wrapper for iconv
