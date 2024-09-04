#!/bin/bash

if [ $# != "1"]
then
	echo "Usage : $0 filename"
	exit 1
fi

[ -f $1 ] || exit 1
scp $1 pi@192.168.0.105:/home/pi/Desktop/veda
