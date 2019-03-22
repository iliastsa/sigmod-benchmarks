//
// Created by Ilias on 22-Mar-19.
//

#include <unistd.h>
#include "ReadReader.h"

ReadReader::~ReadReader() {

}

unsigned char* ReadReader::read_file() {
    unsigned char* buf = static_cast<unsigned char*>(malloc(file_size));

    uint64_t bytes_read = 0;
    while (bytes_read < file_size) {
        bytes_read += read(fd, buf + bytes_read, file_size - bytes_read);
    }

    return buf;
}
