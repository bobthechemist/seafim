# (SEAFIM) Seabreeze Flame Interface for Mathematica

This program is a simple interface for controlling the Ocean Optics FLAME spectrometer using Mathematica.  It consists of a *driver* which was written in c and a Mathematica package.

The driver is a c program that takes configuration information as options and spits out x,y data.  The x data are the wavelengths and y data are the pixel intensities at those wavelengths.  

## Setup

- Download the seabreeze API from [Ocean Optics website](https://oceanoptics.com/product/seabreeze/#tab-downloads).
- The documentation can be found [here](https://oceanoptics.com/api/seabreeze)
- A fresh (2019) Raspbian install doesn't have libusb-dev, which is needed to compile Seabreeze: `sudo apt-get install libusb-dev`
Install the Seabreeze SDK (I need to remember how) 
- in Log.cpp, delete one of the tabs in lines 166 and 182. Make is set to treat all warnings as errors, and these two indentations throw warnings that cause the compile to fail
- copy os-support/linux/10-oceanoptics.rules to /etc/udev/rules.d and reboot
- for testing use `export LD_LIBRARY_PATH="$PWD/lib"`
- Copy libseabreaze.so to /usr/local/lib and run sudo ldconfig -v 

## Usage

Work in progress

