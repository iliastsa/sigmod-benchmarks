#!/bin/bash

mkdir -p ./bin

g++ -O3 -Wextra -Werror -Wall -I ./include -std=c++17 test/range-partitioning-17.cpp -o ./bin/range17.exe
