#!/bin/sh
make re
rm woody
rm tmp
./woody_woodpacker $1
i=$2
j=0
while test $i != 0
do
    mv woody tmp
    ./woody_woodpacker tmp
    i=$(($i - 1))
	j=$(($j + 1))
	echo $j
done
