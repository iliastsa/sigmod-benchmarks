CC=g++
EXECS=radixsort
OBJECTS=radixsort.o utils.o
SOURCE=radixsort.cpp utils.cpp
HEADERS=utils.h Timer.h
PARAMS=-O3

all: ${EXECS}

radixsort: ${OBJECTS}
	${CC} ${PARAMS} -o radixsort ${OBJECTS}

radixsort.o: radixsort.cpp
	${CC} ${PARAMS} -c radixsort.cpp

utils.o: utils.cpp
	${CC} ${PARAMS} -c utils.cpp

clean:
	rm ${EXECS} ${OBJECTS}
