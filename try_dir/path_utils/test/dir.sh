#!/usr/bin/env bash

result=""

for i in {1..255}; do
	result+='c'
done 

#echo "$result"

#mkdir $result

while  mkdir "$result"; do 

	cd "$result"

	myvar=$(pwd)
	size="${#myvar}"
	echo "$size"

done 


