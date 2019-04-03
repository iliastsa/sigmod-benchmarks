#!/bin/bash

mkdir -p ./bin

g++ -I ./include -std=c++17 test/range-partitioning-17.cpp -o ./bin/a.out
