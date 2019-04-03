#include "utils.h"
#include "Constants.h"

void readFile(uint32_t num_tuples, Tuple *tuples, uint32_t fd, uint32_t file_size) {
    unsigned char mem[101];
    uint64_t sz, offset = 0;
    size_t line = 100;
    ssize_t bytes_read=-1, total_bytes_read = 0;

    while (total_bytes_read < file_size){
        while ( (bytes_read = read(fd, mem, line)) > 0){
            total_bytes_read += bytes_read;
            memcpy(tuples[offset].key, &mem[0], Constants::KEY_SIZE);
            // memcpy(tuples[offset].payload, &mem[10], Constants::PAYLOAD);
            tuples[offset].rowID = offset;
            // cout << tuples[offset]->key << tuples[offset]->payload << endl;
            offset++;
            memset(mem, 0, sizeof(mem));
        }

        cout << "Read --> " << total_bytes_read << endl;
    }

}


void radixsort(Tuple *tuples, uint32_t num_tuples){

    //for ascii numberOfBins = 128
    uint32_t numberOfBins = 256;
    Tuple *outputArray = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));
    for (uint32_t i = 0; i < num_tuples; i++) {
        outputArray[i].key = static_cast<unsigned char*>(malloc(Constants::KEY_SIZE * sizeof(char)));
        // outputArray[i].payload = static_cast<unsigned char*>(malloc(Constants::PAYLOAD * sizeof(char)));
    }

    uint64_t count[numberOfBins] = {0};
    int32_t byteMask = 9;

    uint64_t startOfBin[numberOfBins], endOfBinIndex, index;

    //end processing digits when all the mask bits have been processed and shifted out, leaving no bits set in the byteMask
    while (byteMask >= 0){

        // cout << "byteMask:" << byteMask << endl;
        memset(count, 0, sizeof(count));

         // Scan Key array and count the number of times each digit value appears - i.e. size of each bin
        for (uint32_t current = 0; current < num_tuples; current++){
            count[tuples[current].key[byteMask]]++;
        }

        startOfBin[0] = 0;
        for (uint32_t i = 1; i < numberOfBins; i++)
            startOfBin[i] = startOfBin[i - 1] + count[i - 1];

        for (uint32_t current = 0; current < num_tuples; current++){

            endOfBinIndex = tuples[current].key[byteMask];
            // cout << endOfBinIndex << endl;
            index = startOfBin[endOfBinIndex];
            // cout << index << endl;
            memcpy(outputArray[index].key, tuples[current].key, Constants::KEY_SIZE);
            // memcpy(outputArray[index].payload, tuples[current].payload, Constants::PAYLOAD);
            outputArray[index].rowID = tuples[current].rowID;

            startOfBin[endOfBinIndex]++;
        }

        Tuple *tmp = tuples;
        tuples = outputArray;
        outputArray = tmp;     // swap input and output arrays
        byteMask--;
    }

    // for (uint32_t i = 0; i < num_tuples; i++)
    //     free(outputArray[i].key);
    // free(outputArray);
}

void radixsort2(Tuple *tuples, uint32_t num_tuples){

    //for ascii numberOfBins = 128
    uint32_t numberOfBins = 256;
    Tuple *outputArray = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));
    for (uint32_t i = 0; i < num_tuples; i++) {
        outputArray[i].key = static_cast<unsigned char*>(malloc(Constants::KEY_SIZE * sizeof(char)));
        // outputArray[i].payload = static_cast<unsigned char*>(malloc(Constants::PAYLOAD * sizeof(char)));
    }


    uint64_t count[Constants::KEY_SIZE][numberOfBins];
    uint64_t startOfBin[Constants::KEY_SIZE][numberOfBins];

    for (uint32_t i = 0; i < Constants::KEY_SIZE; i++) {
        for (uint32_t j = 0; j < numberOfBins; j++) {
            startOfBin[i][j] = 0;
            count[i][j] = 0;
        }
    }

    for (uint32_t i = 0; i < Constants::KEY_SIZE; i++)
        for (uint32_t j = 0; j < num_tuples; j++)
            count[i][tuples[j].key[i]]++;


    for (uint32_t i = 0; i < Constants::KEY_SIZE; i++) {
        startOfBin[i][0] = 0;
        for (uint32_t j = 1; j < numberOfBins; j++)
            startOfBin[i][j] = startOfBin[i][j-1] + count[i][j-1];
    }

    uint64_t *startOfBinLoc, endOfBinIndex, index;

    int32_t byteMask = Constants::KEY_SIZE - 1;

    while (byteMask >= 0){

        // cout << "byteMask:" << byteMask << endl;

        startOfBinLoc = startOfBin[byteMask];
        for (uint32_t current = 0; current < num_tuples; current++){

            endOfBinIndex = tuples[current].key[byteMask];
            // cout << endOfBinIndex << endl;
            index = startOfBinLoc[endOfBinIndex];
            // cout << index << endl;
            memcpy(outputArray[index].key, tuples[current].key, Constants::KEY_SIZE);
            // memcpy(outputArray[index].payload, tuples[current].payload, Constanst::PAYLOAD);
            outputArray[index].rowID = tuples[current].rowID;
            startOfBinLoc[endOfBinIndex]++;
        }

        Tuple *tmp = tuples;
        tuples = outputArray;
        outputArray = tmp;     // swap input and output arrays
        byteMask--;
    }


    // for (uint32_t i = 0; i < num_tuples; i++) {
    //     free(outputArray[i].payload);
    //     free(outputArray[i].key);
    // }
    // free(outputArray);
}
