#!/bin/bash

START_TIME=$SECONDS
cd build/
cmake ..
cmake --build . -- -j8
#scons --jobs=8
ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo $ELAPSED_TIME

