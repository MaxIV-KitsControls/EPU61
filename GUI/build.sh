#!/bin/bash


CD=`dirname $0`


taurusuic4  -o undulatorDesigner.py Undulator.ui

. $1

rm gui
rm -rf out
mkdir out

for file in undulator.py undulatorDesigner.py
do
	sed -e "s!\$undulator!$UNDULATOR!g" \
		-e "s!\$gapz1!$GAPZ1!g" \
		-e "s!\$gapz2!$GAPZ2!g" \
		-e "s!\$gapz3!$GAPZ3!g" \
		-e "s!\$gapz4!$GAPZ4!g" \
		-e "s!\$phasex1!$PHASEX1!g" \
		-e "s!\$phasex2!$PHASEX2!g" \
		-e "s!\$phasex3!$PHASEX3!g" \
		-e "s!\$phasex4!$PHASEX4!g" \
		-e "s!\$gearUp!$GEARUP!g" \
		-e "s!\$gearLow!$GEARDOWN!g" \
		-e "s!\$opc!$OPC!g"  < $file > ./out/$file
done

chmod a+x ./out/undulator.py
ln -s ./out/undulator.py ./gui
