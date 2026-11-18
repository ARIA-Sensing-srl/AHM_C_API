# C/C++ API for interfacing AHM family devices
This library implements routine for configuring and get data from the devices.

## Supported modules are:
* AHM2D
* AHM2DL
* AHM3D
* AHM2DSC

## Supported communication channels
* **Serial (UART)**: this is the default and always enable interface type
* **SPI**: (disable default), implemented using FTDI libraries (test with C232HM-DDHSL-0 cable) or wiringPi libraries (raspberry pi only)

## Autotool example
```bash
	git clone https://github.com/ARIA-Sensing-srl/AHM_C_API.git
	cd AHM_C_API
	mkdir build && cd build
	../configure
	make
	./AHMDataCapture uart /dev/ttyUSBx
```

Optional support for SPI (FTDI cable), required libraries
* mpsse
* ftd2x 

```bash
	./configure --enable-spi=ftdi 
```
SPI using wiring PI (raspberry only), wiringPi libraries requried
```bash
	./configure --enable-spi=wiringpi
```

##CMake
```bash
	git clone https://github.com/ARIA-Sensing-srl/AHM_C_API.git
	cd AHM_C_API
	mkdir build && cd build
	cmake ..
	cmake --build ..
	./AHMDataCapture uart /dev/ttyUSBx
```
To enable SPI support
```bash
	cmake -DUSE_SPI=1 ..
```




##Examples: 
* **AHMDataCapture**: shows how to configure the device and retrieve data. Data are stored into txt files, one raw for acquisition, commad-separated-value, organized as I0, Q0, .... In, Qn where I is the real part and Q is the imaginary part of the sample n. The example retrieve the final configuration and save all the data into a JSON formatted file
* **AHMImageCapture**: shows how to configure the device and get computed image. Outcome is stored into a comma-separated-value file.
* **AHMTracker2D**: enable the embedded tracking algorithm (AHM2D, AHM2DL and AHM2DSC only). Example shows the position of the first target retrieved

	
## Run the example using FTDI interface ##
Run the example using FTDI cable, requires manual detach of the FTDI cable (C232HM-DDHSL-0 tested) from the default driver (serial port). The procedure may vary between distro and requries superuser access.
* Attach the device
* Run
```bash
	sudo dmesg
```
* Look close to the message tail for this pattern
```bash
	[23135.546617] ftdi_sio ttyUSB0: FTDI USB Serial Device converter now disconnected from ttyUSB0
	[23135.546643] ftdi_sio 4-1.1:1.0: device disconnected
	[23248.897372] ftdi_sio 4-1.1:1.0: FTDI USB Serial Device converter detected
	[23248.897425] usb 4-1.1: Detected FT232H
```
* In this case device ID is 4-1.1:1.0
* Run
```bash
	sudo chmod o+w /sys/bus/usb-serial/drivers/generic/module/drivers/usb:ftdi_sio/unbind
	echo -n "4-1.1:1.0" > /sys/bus/usb-serial/drivers/generic/module/drivers/usb:ftdi_sio/unbind
```
NOTE: Almalinux map the unbind into
```bash
	/sys/bus/usb/drivers/ftdi_sio/unbind
```
* Once unbinded, run the example:
```bash
	./AHMDataCapture spi 0
```
## Run the example using wiring Pi
This requires spidev interface enable to expose spidevx.x
```bash
	./AHMDataCapture spi /dev/spidevx.x
```
	
	

