#!/bin/bash

dbg_port="/dev/tty_DBG_0"

if [[ $1 != "" ]] ; then
  if [[ $1 == "clean" ]] ; then
    rm data.bin data1.bin pv_uart.cfg
    exit
  fi
  if (( $1 == 0 )) ; then
    echo "Неверный делитель"
    exit
  fi
  (( speed=144000000 / $1 ))
else
  (( speed=72000000 ))
fi

stty -F $dbg_port 115200
cat $dbg_port > data.bin &

echo "Wait for start transmission"
while (( `du data.bin -b | awk '{print $1}'` < 5 )) ; do
  echo -ne '\r|'
  sleep 0.2
  echo -ne '\r/'
  sleep 0.2
  echo -ne '\r-'
  sleep 0.2
  echo -ne '\r\\'
  sleep 0.2
done
echo -ne "\r \r"

echo "Wait for finish transmission"
sz_prev=0
sz_cur=1
while (( $sz_prev != $sz_cur )) ; do
  sz_prev=$sz_cur
  (( sz_cur = `du data.bin -b | awk '{print $1}'` ))
  sleep 2
done

kill %1

echo "Done: " $sz_cur " samples"

mv -f data.bin data1.bin
tail -c +10 data1.bin > data.bin

speedstr=$(echo -n $speed | awk '{printf "\\\\x%x\\\\x%x\\\\x%x\\\\x%x\\\\0\\\\0\\\\0\\\\0", $1%256, ($1/256%256), ($1/256/256%256), ($1/256/256/256%256)  }')
fmtstr="s/\\(decoder0\\\\option0\\\\value\\)=.*/\1=@ByteArray($speedstr)/"
sed pv_uart_template.cfg -e $fmtstr > pv_uart.cfg
pulseview -c -i data.bin -I binary:numchannels=5:samplerate=$speed -s pv_uart.cfg