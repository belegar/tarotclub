#!/bin/bash
 
# gen.sh base_directory


for entry in "$1"/**/* "$1"/*
do
    if [ -f "$entry" ]
    then
        list+=" ${entry#$1}"
    fi
done

echo "$list"


