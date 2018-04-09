# MacWifi
A wifi system menu extension for 68k Macs.

## Building & Installing
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