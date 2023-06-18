#!/bin/bash 

cd ..
#for n in {2..20} 50 100 500 1000 2000 5000 10000
for n in {2..20} 50 100 500 1000 
do 
	echo $n
	./tests/prog_test $n
done 
