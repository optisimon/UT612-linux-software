# UT612-linux-software
This is a work in progress to add linux support for the  UNI-T UT612 LCR meter


## Current status:

 - Can run live on a linux system against the UT612 LCR Meter.
 - Can run against offline data sets collected using usb sniffing on a windows computer.
 - Interprets the primary measurement value for all resistance measurements
 - Keeping the likely readout bug in the windows software at the moment until
   I have verified with large resistors that the LCR meter is right and the
   windows software wrong.
 - Handles readings slightly different than the windows software, since that
   felt much more sensible. That affects one reading when measurement mode
   on the LCR meter is switched. Will have to record a movie of the LCR
   meter and the windows software side by side to see if I'm right or wrong.
 - Interprets the primary measurement value for some capacitance and inductance
   ranges.
 - Won't work for most inductance and capacitance measurements until more
   usb snoops with more components have been collected and analyzed


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

