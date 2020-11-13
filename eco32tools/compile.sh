#!/bin/sh
BIN=.
LIB=lib 
file=$1
$BIN/refspl $file.spl $file.s 
$BIN/as -o $file.o $file.s
$BIN/ld -s $LIB/stdalone.lnk -L$LIB -o $file.x $LIB/start.o $file.o -lsplrts
$BIN/load $file.x $file.bin
       
