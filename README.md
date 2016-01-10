# UT612-linux-software
This is a work in progress to add linux support for the  UNI-T UT612 LCR meter


## Current status:

 - currently not running live, only running on binary data dumps.
 - Parses binary data sets collected using usb sniffing on a windows computer
 - Also parses binary data collected on a linux computer using an USB-HID-Uart
   example application
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
instead of a clear text protocol.

## License

Not detecided yet, just clearly mention me as the source if this gets reused.

This repository is likely to be tainted by third party source code if it's way
easier or feels more reliable to use those than it would be to use libusb directly,
unless some slightly newer kernel natively supports the USB-HID-Uart chip used by
this product.

Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
