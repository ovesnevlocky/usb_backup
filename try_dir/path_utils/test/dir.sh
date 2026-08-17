#!/usr/bin/env bash

result=""
count=0

for i in {1..25}; do
	result+='c'
done 

rm -r "$result"

#echo "$result"

#mkdir $result

while  mkdir "$result"; do 
	
	let "count++"
	touch "$count"
	
	cd "$result"
	myvar=$(pwd)
	size="${#myvar}"
	echo "$size"
	touch "$size"	
#	if [ "$count" -eq 300 ]; then
#		break
#	fi
done 


