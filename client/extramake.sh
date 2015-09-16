#!/bin/bash 

# Arguments
# $1 : source directory
# $2 : build directory

# copy license file to build directory
mkdir -p "$2"
cp "$1/../../LICENSE" "$2/"
