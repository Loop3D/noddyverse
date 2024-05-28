#!/bin/bash
# Basic while loop
counter=1
while [ $counter -le 100 ]
do
	NOW=$(date +"%y-%m-%d-%H-%M-%S-%N")
	#echo $NOW
    ./noddy dikefault.his $NOW.his RANDOM 
    ((counter++))
done
