#!/usr/bin/bash
EXPECTED_ARGS=1
if [ $# -lt $EXPECTED_ARGS ]
then
  echo "Too few arguments. Usage: `basename $0` filename {arg, arg2, ...}"
  echo "Filename is the PintOS userprogram you want to execute."
  echo 
  exit
fi
# File name should always be first argument.
filename=$1 
# Arguments passed to userprogram, includes filename too.
arguments=$@			
cd src/userprog/build/;
str="pintos -p ../../examples/$filename -a $filename -v -k --fs-disk=2 -- -f -q run '$arguments'"
eval $str
