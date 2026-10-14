C/C++ API for interfacing AHM family devices
This library implements routine for configuring and get data from the devices.

Supported modules are:
AHM2D
AHM2DL
AHM3D

Supported communication channels:
Serial (UART): this is the default and always enable interface type
SPI: (disable default), implemented using FTDI libraries (test with C232HM-DDHSL-0 cable) or wiringPi libraries (raspberry pi only)

Supported autotools and cmake

CMake auto detect libraries (FTDI / wiringPi) and automatically enable support for SPI

Examples: 
AHMDataCapture: shows how to configure the device and retrieve data. Data are stored into txt files, one raw for acquisition, commad-separated-value, organized as I0, Q0, .... In, Qn where I is the real part and Q is the imaginary part of the sample n. The example retrieve the final configuration and save all the data into a JSON formatted file
AHMImageCapture: shows how to configure the device and get computed image. Outcome is stored into a comma-separated-value file.


##BUILD examples Linux###
	git clone https://github.com/ARIA-Sensing-srl/AHM_C_API.git
	cd AHM_C_API
	mkdir build && cd build
	../configure
	make
	./AHMDataCapture uart /dev/ttyUSBx

##Windows (msys)
	cd AHM_C_API
	mkdir build && cd build
	../configure
	make
	./AHMDataCapture uart \\\\.\\COMx
	
## SPI support ######
	./configure --enable-spi=ftdi (require mpsse and ftd2x libraries)
	./configure --enable-spi=wiringpi (requires wiringPi libraries, raspberry pi only)
	
## Run the example using FTDI interface ##
Run the example using FTDI cable, requires manual detach of the FTDI cable (C232HM-DDHSL-0 tested) from the default driver (serial port). The procedure may vary between distro and requries superuser access.
1. attach the device
2. Run
	sudo dmesg
3. Look close to the message tail for this pattern
	[23135.546617] ftdi_sio ttyUSB0: FTDI USB Serial Device converter now disconnected from ttyUSB0
	[23135.546643] ftdi_sio 4-1.1:1.0: device disconnected
	[23248.897372] ftdi_sio 4-1.1:1.0: FTDI USB Serial Device converter detected
	[23248.897425] usb 4-1.1: Detected FT232H
4. In this case device ID is 4-1.1:1.0
5. Run
	sudo chmod o+w /sys/bus/usb-serial/drivers/generic/module/drivers/usb:ftdi_sio/unbind
	echo -n "4-1.1:1.0" > /sys/bus/usb-serial/drivers/generic/module/drivers/usb:ftdi_sio/unbind
	
NOTE: Almalinux map the unbind into
	/sys/bus/usb/drivers/ftdi_sio/unbind

6. Once unbinded, run the example:
	./AHMDataCapture spi 0
	
## Run the example using wiring Pi
This requires spidev interface enable to expose spidevx.x
	./AHMDataCapture spi /dev/spidevx.x
	
	
	

