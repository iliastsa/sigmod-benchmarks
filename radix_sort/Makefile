
.PHONY: all
all:
	make hrange17
	make vrange8

hrange17:
	mkdir -p ./bin
	g++ -ofast -Wextra -Werror -Wall -I ./include -std=c++17 test/horizontal_range_17.cpp -o ./bin/hrange17.exe

vrange8:
	mkdir -p ./bin
	g++ -mavx512f -ofast -Wextra -Werror -Wall -I ./include -std=c++17 test/vertical_range_8.cpp -o ./bin/vrange8.exe

.PHONY: clean
clean:
	rm -rf ./bin
