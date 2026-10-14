AC_INIT([AHM_C_API], [1.0])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])


AC_CANONICAL_HOST

build_linux=no
build_windows=no
build_mac=no

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])


AC_ARG_ENABLE([spi],
    AS_HELP_STRING([--enable-spi], [Enable SPI support no/ftdi/wiringpi @<:@default=no@:>@]),
    [case "${enableval}" in
        no) enable_spi=false use_ftdi=false use_wiringpi=false;;
        ftdi)  enable_spi=true use_ftdi=true use_wiringpi=false;;
        wiringpi)  enable_spi=true use_ftdi=false use_wiringpi=true;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-spi]) ;;
     esac],[enable_spi=false use_ftdi=false use_wiringpi=false])
     
AM_CONDITIONAL([ENABLE_SPI], [test x$enable_spi = xtrue])
AM_CONDITIONAL([USE_FTDI], [test x$use_ftdi = xtrue])
AM_CONDITIONAL([USE_WIRINGPI], [test x$use_wiringpi = xtrue])

if test x$use_wiringpi = xtrue ; then
	AC_CHECK_LIB(wiringPi, wiringPiSPIDataRW, [], [
	echo "wiringPi library not found."
	exit -1
	])
fi

if test x$use_ftdi = xtrue ; then
	AC_CHECK_LIB(mpsse, FT_OpenChannel, [], [
	echo "MPSSE lib not found."
	exit -1
	])
	AC_CHECK_LIB(ftd2xx, FT_Write, [], [
	echo "ftd2xx lib not found."
	exit -1
	])
fi
AC_OUTPUT

