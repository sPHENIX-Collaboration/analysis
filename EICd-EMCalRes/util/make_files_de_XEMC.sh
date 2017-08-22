#!/bin/sh

#this is just like make_files_de.sh,
#except that, instead of being fully differential in pseudorapidity,
#the events generated for each subdetector are clumped.
#in fact! i use the rcli0 files created by make_files_de.sh

rm ../data/rcli0_XEMC/*
rm ../data/rcli1_XEMC/*
rm ../data/de_XEMC/*

for filename in $(ls ../data/rcli0); do
	#generate the EEMC, CEMC, and FEMC-grouped RCLI0 file for each and every fully differential RCLI0 file.
	#this is a little inefficient, but it still takes effectively zero time.
	#the idea is, the looped flavor and momentum values can be changed to whatever,
	#and the subdetector-grouped RCLI0 files will all still come out fine.
	
	code_flavor=${filename%???????}
	code_momentum=${filename#????????}
	
	cat \
		../data/rcli0/${code_flavor}132${code_momentum} \
		../data/rcli0/${code_flavor}124${code_momentum} \
		../data/rcli0/${code_flavor}116${code_momentum} \
	> ../data/rcli0_XEMC/${code_flavor}${code_momentum}_EEMC
	
	cat \
		../data/rcli0/${code_flavor}104${code_momentum} \
		../data/rcli0/${code_flavor}000${code_momentum} \
		../data/rcli0/${code_flavor}008${code_momentum} \
	> ../data/rcli0_XEMC/${code_flavor}${code_momentum}_CEMC
	
	cat \
		../data/rcli0/${code_flavor}016${code_momentum} \
		../data/rcli0/${code_flavor}024${code_momentum} \
		../data/rcli0/${code_flavor}032${code_momentum} \
	> ../data/rcli0_XEMC/${code_flavor}${code_momentum}_FEMC
done

for filename in $(ls ../data/rcli0_XEMC); do
	#the same thing as the last loop, but
	#go through *those* files
	#and integrate over the momentum indices
	#to generate RCLI1 and DE files.
	
	code_flavor=${filename:0:5}
	code_subdetector=${filename:9:5}
	
	#first, generate RCLI1 files
	
	cd ../data/rcli0_XEMC/
	for f in $(ls -1 ${code_flavor}*${code_subdetector}); do
		code_momentum_big=${f:5:2}
		code_momentum_small=${f:7:2}
		momentum=$(printf "%s+%s/100\n" $code_momentum_big $code_momentum_small | bc -lq)
		printf "%s %.2f\n" $f $momentum
	done > ../rcli1_XEMC/${code_flavor}${code_subdetector}
	cd ../../util
	
	#then, generate DE files
	
	root -l -q -b -x make_file_de.C\(\"../data/de_XEMC/DE${code_flavor}${code_subdetector}.root\"\)
done
