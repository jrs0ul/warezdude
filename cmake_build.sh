#!/bin/bash

START_TIME=$SECONDS
cd build/
cmake ..
cmake --build . -- -j8
mv ../CQEditor ../editor/
ELAPSED_TIME=$(($SECONDS - $START_TIME))
echo $ELAPSED_TIME

