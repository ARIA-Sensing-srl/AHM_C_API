cmake_minimum_required(VERSION 3.15)
project(AHMDataGrap)
include(CheckLibraryExists)

###############################################################################
## file globbing ##############################################################
###############################################################################

# these instructions search the directory tree when CMake is
# invoked and put all files that match the pattern in the variables
# `sources` and `data`
file(GLOB_RECURSE sources     inc/*.h radar_data/*.h radar_handlers/*.h uart/*.h wiringPiSource/*.h ahm_cmds/*.h)
list (APPEND sources radar_data/radarData.cpp 
	radar_data/radarDataVolume.cpp 
	radar_handlers/radar_AHM2D.cpp 
	radar_handlers/radar_AHM3D.cpp 
	radar_handlers/radar_AHM2DL.cpp 
	radar_handlers/radar_api_base.cpp 
	src/example.cpp 
	src/f16tosingle.cpp 
	src/msgEncoderDecored.cpp 
	uart/radarif_ll.cpp
	uart/radarif_ll_uart.cpp
	)
	
set(INCDIR inc radar_data ahm_cmds radar_handlers uart wiringPiSource )
# you can use set(sources src/main.cpp) etc if you don't want to
# use globbing to find files automatically

if (MSVC)
list (APPEND sources wiringPiSource/winSerial.c)
add_definitions(-DWIN)
else (MSVC)
list (APPEND sources wiringPiSource/wiringSerial.c)
check_library_exists(mpsse FT_OpenChannel "" HAVE_MPSSE_LIB)
check_library_exists(ftd2xx FT_Write "" HAVE_FTD2XX)
check_library_exists(wiringPi wiringPiSPIDataRW "" HAVE_WIRINGPI)

if ((HAVE_MPSSE_LIB AND HAVE_FTD2XX) OR HAVE_WIRINGPI)
list (APPEND sources spi/radarif_ll_spi.cpp)
list (APPEND INCDIR spi)
add_definitions(-DCONFIG_USE_SPI)
message(STATUS "Enable FTDI cable support")

if (HAVE_WIRINGPI)
list (APPEND sources spi/spi_interface.cpp)
else (HAVE_WIRINGPI)
list (APPEND sources spi/spi_interface_FTDI.cpp)
endif (HAVE_WIRINGPI)


endif ((HAVE_MPSSE_LIB AND HAVE_FTD2XX) OR HAVE_WIRINGPI)
endif (MSVC)

###############################################################################
## target definitions #########################################################
###############################################################################

# add the data to the target, so it becomes visible in some IDE
add_executable(AHMDataGrap ${sources} )
if ((HAVE_MPSSE_LIB AND HAVE_FTD2XX) OR HAVE_WIRINGPI)
if (HAVE_WIRINGPI)
	target_link_libraries(AHMDataGrap -lwiringPi)
else (HAVE_WIRINGPI)
	target_link_libraries(AHMDataGrap -lmpsse -lftd2xx)
endif (HAVE_WIRINGPI)
endif ((HAVE_MPSSE_LIB AND HAVE_FTD2XX) OR HAVE_WIRINGPI)


# this lets me include files relative to the root source directory with a <> pair
target_include_directories(AHMDataGrap PUBLIC ${INCDIR} )



