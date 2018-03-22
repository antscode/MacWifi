cmake_minimum_required(VERSION 2.8)

add_executable(MacWifi
	sysmenu.c
	resources.r)

set_target_properties(MacWifi PROPERTIES
	OUTPUT_NAME MacWifi.flt
	LINK_FLAGS -Wl,--mac-flat)

# Put everything together
add_custom_command(
	OUTPUT MacWifi.bin MacWifi.dsk
	COMMAND ${REZ} -I ${REZ_INCLUDE_PATH}
					${CMAKE_CURRENT_SOURCE_DIR}/resources.r
					-o MacWifi.bin
					--cc MacWifi.dsk
					--cc MacWifi.init
					-t INIT -c MWFI
	DEPENDS MacWifi resources.r)

add_custom_target(MacWifi_INIT ALL DEPENDS MacWifi.dsk)