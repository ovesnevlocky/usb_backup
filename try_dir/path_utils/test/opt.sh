#!/usr/bin/env bash

echo $1
#-c option enables us to complile a program without main
cc "$1" -c  -o ./files/test.o
objdump -S -M intel ./files/test.o > ./files/0.txt 


cc "$1" -c -O3 -o  ./files/testO3.o
objdump -S -M intel ./files/testO3.o > ./files/3.txt 


