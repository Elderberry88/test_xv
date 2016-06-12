#!/bin/env bash
UPROGS=$(make -pn TOOLPREFIX= | grep \^UPROGS | cut -s -d" " -f 1,2 --complement)
extra_progs=
dos2unix ./sign.pl
rm -f fs.img
make TOOLPREFIX=
for prog in $extra_progs;
do
	echo Adding $(basename "$prog") to FS...
	cp "$prog" .
	UPROGS="$UPROGS $(basename "$prog")"
done;
