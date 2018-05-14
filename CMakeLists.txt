cmake_minimum_required(VERSION 2.8)

add_executable(MacWifiINIT
	sysmenu.cpp
	Util.cpp
	ShowInitIcon.c
	resources.r
)

target_link_libraries(MacWifiINIT)

set_target_properties(MacWifiINIT PROPERTIES
    COMPILE_OPTIONS -ffunction-sections # make things smaller

    # set a linker flag that says we want a flat piece
    # of code in a data file, specify entry point,
    # and add -Wl,-gc-sections to make things smaller.
    LINK_FLAGS "-Wl,--mac-flat -Wl,-gc-sections"
)

# wrap the compiled INIT into a resource
add_custom_command(
    OUTPUT MacWifiINIT.rsrc.bin
    COMMAND ${REZ} -I ${REZ_INCLUDE_PATH}
                    ${CMAKE_CURRENT_SOURCE_DIR}/resources.r
                    -o MacWifiINIT.rsrc.bin                    
    DEPENDS MacWifiINIT resources.r
)

# Now build the application
add_application(MacWifi
	MacWifi.cpp
	resources.r
	Modules/WifiModule.cpp
	Modules/VM300.cpp
	Comms.cpp
	Util.cpp

    # the separately compiled WDEF resource
    ${CMAKE_CURRENT_BINARY_DIR}/MacWifiINIT.rsrc.bin   

	TYPE "appe" # application extension
	CREATOR "MWFI"
)

target_link_libraries(MacWifi MacHTTP MacTCPHelper)

# again, add some options to make things smaller.
set_target_properties(MacWifi PROPERTIES COMPILE_OPTIONS -ffunction-sections)
set_target_properties(MacWifi PROPERTIES LINK_FLAGS "-Wl,-gc-sections")
