AM_CPPFLAGS=-I$(srcdir)/ahm_cmds \
 -I$(srcdir)/inc \
 -I$(srcdir)/radar_data \
 -I$(srcdir)/radar_handlers \
 -I$(srcdir)/spi \
 -I$(srcdir)/src \
 -I$(srcdir)/uart \
 -I$(srcdir)/wiringPiSource

bin_PROGRAMS = AHMCaptureData AHMCaptureImage

if ENABLE_SPI
  AM_CPPFLAGS+=-DCONFIG_USE_SPI
endif

if WINDOWS
  AM_CPPFLAGS+=-DWIN
endif

AUTOMAKE_OPTIONS = subdir-objects

app_common_sources = \
	radar_data/radarData.cpp \
	radar_data/radarDataVolume.cpp \
	radar_handlers/radar_AHM2D.cpp \
	radar_handlers/radar_AHM3D.cpp \
	radar_handlers/radar_AHM2DL.cpp \
	radar_handlers/radar_api_base.cpp \
	src/f16tosingle.cpp \
	src/msgEncoderDecored.cpp \
	uart/radarif_ll.cpp \
	uart/radarif_ll_uart.cpp



if ENABLE_SPI
app_common_sources += spi/radarif_ll_spi.cpp
endif

if USE_FTDI
app_common_ldadd =-lmpsse -lftd2xx
app_common_sources +=spi/spi_interface_FTDI.cpp
endif

if WINDOWS
app_common_sources += wiringPiSource/winSerial.c
else
app_common_sources += wiringPiSource/wiringSerial.c
endif

if USE_WIRINGPI
app_common_ldadd=-lwiringPi
app_common_sources +=spi/spi_interface.cpp
endif	

AHMCaptureData_SOURCES = src/example.cpp $(app_common_sources)
AHMCaptureData_LDADD = $(app_common_ldadd)

AHMCaptureImage_SOURCES = src/exampleimage.cpp $(app_common_sources)
AHMCaptureImage_LDADD = $(app_common_ldadd)
