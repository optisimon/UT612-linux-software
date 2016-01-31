# UT612-linux-software
This project adds Ubuntu 14.04 support for the UNI-T UT612 LCR meter.
It may work with other Debian based distros as well.

It's still under active development, but is completely usable in its
current form.


## Current status:

 - Can run live on a linux system against the UT612 LCR Meter.
 - Decodes both the primary measurement value and secondary measurement
   value for all ranges
 - Correctly displays high MOhm readings the same way as the LCR meter
   does (UNI-T:s windows software v1.00 shows incorrect values)
 - Correctly handles data sent during swithes of measurement mode
   (UNI-T:s windows software v1.00 typically logs one erroneous value each
   time that happens).
 - Correctly displays theta when meter in auto mode (matches meter reading,
   which is different from presentation and logging in UNI-T:s windows
   software v1.00)
 - Can run against offline data sets collected using usb sniffing on a
   windows computer (i.e. to get bit-identical input to that used by
   UNI-T:s windows software)


## Installing:

I assume that you are running ubuntu 14.04, already have gnu make and
a compiler (or else it may be enough to run `sudo apt-get install build-essential`)

```
git clone https://github.com/optisimon/UT612-linux-software.git
cd UT612-linux-software/src
sudo make prepare && make && sudo make install
```

### IMPORTANT: Disabling cp210x

In ubuntu 14.04, the cp210x driver is incorrectly trying to grab the LCR
meter, and fails to open it (since that driver don't support the USB chip).
To disable that driver from interfering, you have to follow the instructions
in the file `src/blacklist-cp210x.conf` to disable that driver from trying to
load that specific chip. To test if that would help, you could try to remove
the cp210x module altogether by running `rmmod cp210x`.

You will have to unplug and replug the LCR meter if it already was
connected during the installation. Note that turning the meter off and
then on again won't be enough.


## Usage:

Start the UNI-T UT612 LCR Meter, and turn on USB mode.
An icon with a small computer screen with an S inside it should appear
in the top left corner of the LCR meters display.

Start the ut612 tool by running:
```
ut612
```

The tool will print one line of measurement data each time the LCR meter
takes a new reading. All values on each line is tab-separated.

Typical output:

Typical output (when pasted into a table):

NO | Time | MMode | MValue | MUnit | SMode | SValue | SUnit | Freq
--- | ---- | ----- | ------ | ----- | ----- | ------ | ----- | ----
0 | 2016-01-16 20:48:12.414 | Cs | 94.68 | uF | ESR | 1.868 | Ohm | 1KHz
1 | 2016-01-16 20:48:12.916 | Cs | 94.68 | uF | ESR | 1.868 | Ohm | 1KHz
2 | 2016-01-16 20:48:13.439 | Cs | 94.68 | uF | ESR | 1.868 | Ohm | 1KHz
3 | 2016-01-16 20:48:13.921 | Cs | 94.68 | uF | ESR | 1.868 | Ohm | 1KHz
...


## Uninstalling

If you for some reason don't want to have the software installed any longer,
just go into the source folder and uninstall it:

```
cd UT612-linux-software/src
sudo make uninstall
```


## Disclaimer

Use at your own risk, especially since no protocol specifications or
source code was available, and they've choosen to use a binary protocol
instead of a clear text protocol. Something as simple as a low battery warning
could potentially stop me from parsing the data :)


## Other software
I'm using the hidapi library from Signal 11 Software (http://github.com/signal11/hidapi)
to communicate with the UT612 LCR Meter. The main reasons for using that is

1) Nice license options (GPL3, BSD, as well as just embedding their code directly)

2) The library can communicate to hid devices under linux using either libusb or the
   kernels hidraw interface.

3) I chose to only use the hidraw interface, but still have the hidapi library as a
   middle man if I ever would port the software to windows, or would want to use the
   libusb library as a backend instead (for embedded platforms running ancient kernel
   versions)

4) Nothing I've tried could get the UART to appear as a regular device node under
   ubuntu 14.04.


## License

Just clearly mention me as the source if this gets reused, and
don't remove any of the copyright notices from any of the source files.

Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)

