AM_CPPFLAGS=-I$(srcdir)/ahm_cmds \
 -I$(srcdir)/inc \
 -I$(srcdir)/radar_data \
 -I$(srcdir)/radar_handlers \
 -I$(srcdir)/spi \
 -I$(srcdir)/src \
 -I$(srcdir)/uart \
 -I$(srcdir)/wiringPiSource

bin_PROGRAMS = AHMDatagrab

if ENABLE_SPI
  AM_CPPFLAGS+=-DCONFIG_USE_SPI
endif

if WINDOWS
  AM_CPPFLAGS+=-DWIN
endif

AUTOMAKE_OPTIONS = subdir-objects

AHMDatagrab_SOURCES = \
	radar_data/radarData.cpp \
	radar_data/radarDataVolume.cpp \
	radar_handlers/radar_AHM2D.cpp \
	radar_handlers/radar_AHM3D.cpp \
	radar_handlers/radar_AHM2DL.cpp \
	radar_handlers/radar_api_base.cpp \
	src/example.cpp \
	src/f16tosingle.cpp \
	src/msgEncoderDecored.cpp \
	uart/radarif_ll.cpp \
	uart/radarif_ll_uart.cpp
if ENABLE_SPI
AHMDatagrab_SOURCES+=spi/radarif_ll_spi.cpp
endif

if USE_FTDI
AHMDatagrab_LDADD=-lmpsse -lftd2xx
AHMDatagrab_SOURCES+=spi/spi_interface_FTDI.cpp
endif

if WINDOWS
AHMDatagrab_SOURCES+=wiringPiSource/winSerial.c
else
AHMDatagrab_SOURCES+=wiringPiSource/wiringSerial.c
endif

if USE_WIRINGPI
AHMDatagrab_LDADD=-lwiringPi
AHMDatagrab_SOURCES+=spi/spi_interface.cpp
endif	
