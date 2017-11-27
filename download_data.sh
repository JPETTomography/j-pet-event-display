#!/bin/bash

if [ -z $1 ]; then
    OUTPUT=.
else
    OUTPUT=$1
fi

wget http://sphinx.if.uj.edu.pl/framework/Examples/LargeBarrelAnalysis/large_barrel.json -P "${OUTPUT}"