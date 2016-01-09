#!/bin/bash

if [ $# != 1 ] ;
then
  echo "Please supply a filename to a tab separated usb trace"
  exit 1;
fi

FILENAME="$1"


#
# Fields in the windows software:
# No - incrementing by 1 for each readout
# Time - The computers time
# MMode - Ls, Lp, Cs, Cp, Rs, Rp, DCR, Cp, etc (probably measurement mode)
# MValue - "1,1105", OL (Measurement value)
# MUnit - nF, uF, KOhm, MOhm, Ohm, H, etc (Measurement unit)
# SMode - D, ?, Q, ESR (Secondary display mode)
# SValue - "0,0", "-" "6552"
# SUnit "", "Deg"
# Freq 1KHz 10KHz 100KHz 100Hz 120Hz


# Grep header line, remove all fields after wanted, use tr to discard everything except tabs, then count remaining characters
# and add one (cut -f numbers from one, not zero)
FIELD_OFFSET_DATA=$(( $(cat "$FILENAME" | grep Fid_URB_TransferData | sed -e 's/Fid_URB_TransferData.*//' | tr -dc \\t | wc -c) + 1 ))
FIELD_OFFSET_TIMESTAMP=$(( $(cat "$FILENAME" | grep Timestamp | sed -e 's/Timestamp.*//' | tr -dc \\t | wc -c) + 1 ))

cat "$FILENAME" | cut -f $FIELD_OFFSET_DATA

echo "page_screen_output(0);" > delme.m
echo "data=[" >> delme.m
tail -n +2 "$FILENAME" | cut -f $FIELD_OFFSET_DATA | cut -d ',' -f 2 | cut -d ']' -f 1 >> delme.m
echo "];" >> delme.m
echo "ts=[" >> delme.m
tail -n +2 "$FILENAME" | cut -f $FIELD_OFFSET_TIMESTAMP | sed -e 's/^/"/' -e 's/$/"/' >> delme.m
echo "];" >> delme.m

echo "
index=find(data(1:end-1) == 13 & data(2:end) == 10) + 1;
if index(end) == length(data)
  index = index(1:end-1);
end

for n=1:length(index)-1
  d=data(index(n)+1:index(n+1))';
  disp(['n=' num2str(n) ' ==> ' num2str(d)]) 
end

f = fopen('delme.raw', 'w');
fwrite(f, data, 'uint8' );
fclose(f);
" >> delme.m



