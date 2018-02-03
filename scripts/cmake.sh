#!/bin/sh -ex

cmake .
cmake --build .
ctest --output-on-failure .

