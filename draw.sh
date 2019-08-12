#!/bin/bash

name=$1
#echo $name
./drawsystem "$name" "$name.out"
dot  -Tpng   "$name.out" -o "$name.png" 
