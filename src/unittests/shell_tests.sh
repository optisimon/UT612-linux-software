#!/bin/bash
#
# Quick and dirty test script useful while refactoring
#
# Runs all binary input data available (from reference_data/input*.raw)
# through the ut612 utility, and compares the output to a locked down
# output from a previous run.
#

ERRCNT="0"

TMPFILE=`mktemp /tmp/ut612-unittest.XXXXX` || {
	echo "mktemp error";
	exit 1;
}

[ -x ../ut612 ] || {
	echo "../ut612 executable not found"
	exit 1;
}

for input in reference_data/input*.raw ;
do
	echo -n "PROCESSING $input: "

	expected=`echo "$input" | sed -e 's/input/output/'`

	[ -e $expected ] || {
		echo " ERROR - Expected answers file \"$expected\" not found"
		ERRCNT=$(( $ERRCNT + 1 ))
	}

	rm "$TMPFILE"
	../ut612 --parsebinary "$input" &> "$TMPFILE"

	if [ -e "$expected" ] ;
	then
		diff "$expected" "$TMPFILE" > /dev/null && echo EQUAL || {
			echo "DIFFERS :"
			diff "$expected" "$TMPFILE"
			echo -e "\n\n\n"
			ERRCNT=$(( $ERRCNT + 1 ))
		}
	else
		echo "NO REFERENCE FILE (\"$expected\") FOR THIS:"
		cat "$TMPFILE"
	fi

done

rm "$TMPFILE"


[ $ERRCNT == "0" ] || {
	echo "ERRORS ENCOUNTERED"
	exit 1;
}

