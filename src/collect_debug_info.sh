#!/bin/bash
#
# This script is used for collecting system information needed to debug any
# connection problems with the UT612 LCR Meter.
#
# Usage: collect_debug_info.sh [name_of_generated_report_file.log]
#

if [ $# == 1 ] ; then
	LOGFILE="$1"
	DO_DELETE_LOGFILE=0
else
	LOGFILE=`mktemp /tmp/ut612_debug_info_XXXXXXXX.log`
	DO_DELETE_LOGFILE=1
fi

runCommand()
{
	echo "===== [ $* ] =====" >> "$LOGFILE"
	"$@" >> "$LOGFILE" 2>&1
	local STATUS=${PIPESTATUS[0]}
	echo "==================" >> "$LOGFILE"
	return "$STATUS"
}

runCommandTail100()
{
	echo "===== [ $* | tail -n 100 ] =====" >> "$LOGFILE"
	"$@" 2>&1 | tail -n 100 >> "$LOGFILE" 2>&1
	local STATUS=${PIPESTATUS[0]}
	echo "==================" >> "$LOGFILE"
	return "$STATUS"
}

waitForReturnWithPrompt()
{
	echo "=" >> "$LOGFILE"
	echo "= $*" >> "$LOGFILE"
	echo "=" >> "$LOGFILE"
	read -p "$*"
}

dpkg -l git udev usbutils tree lsb-release &> /dev/null || {
	echo "WE NEED TO HAVE PACKAGES FOR udevadm AND lsusb AND tree AND lsb-release INSTALLED" ;
	sudo apt-get install git udev usbutils tree lsb-release ;
}


NOT_ON_BUS=0

date > "$LOGFILE"

runCommand lsb_release -a
runCommand uname -a
runCommand git rev-parse HEAD

waitForReturnWithPrompt "Make sure your UT612 LCR meter is connected to the USB bus, then hit [enter/return]"
sleep 1

runCommand lsusb -d 0x10c4:0xea80 || NOT_ON_BUS=1
runCommand lsusb
runCommand tree /sys/bus/usb
runCommand head -n 100 /etc/udev/rules.d/*ut612*
runCommand ls -l . /usr/local/bin/ut612
runCommand cat /etc/modprobe.d/blacklist-cp210x.conf
runCommand timeout 2s ./ut612 --verbose

waitForReturnWithPrompt "DISCONNECT the cable to your UT612 LCR meter from your computer, then hit [enter/return]"
sleep 1

runCommandTail100 dmesg
runCommand lsusb
udevadm monitor -p >> "$LOGFILE" &
UDEVADM_PID=$!


printNotOnBusMessage()
{
	echo
	echo "ERROR: at least once when your UT612 LCR meter was supposed to be connected"
	echo "to the USB bus, there were no USB chips with the expected VID:PID connected"
	echo "anywhere."
	echo "  This means that they either have changed VID:PID, or even the chip used "
	echo "in the meter, or that there is an electrical connection problem.";
}

ctrl_c()
{
	echo
	echo "Please wait for 3 seconds"
	sleep 1
	kill "$UDEVADM_PID"
	runCommand lsusb -d 0x10c4:0xea80 || NOT_ON_BUS=1
	echo
	if [ "$DO_DELETE_LOGFILE" == 1 ] ; then
		cat "$LOGFILE"
		rm "$LOGFILE"
	else
		[ "$NOT_ON_BUS" == "1" ] && printNotOnBusMessage >> "$LOGFILE"
		echo "Your log file can be found at \"$LOGFILE\""
	fi

	[ "$NOT_ON_BUS" == "1" ] && printNotOnBusMessage

	exit 0
}

trap ctrl_c INT


while true ; do
	waitForReturnWithPrompt "RECONNECT the cable to your UT612 LCR meter from your computer, then hit Ctrl-C"
done

