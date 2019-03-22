#ifndef SIGMODBENCHMARKS_FILEREADER_H
#define SIGMODBENCHMARKS_FILEREADER_H

#include <string>
#include <stdint-gcc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

class FileReader {
    typedef std::string string;
protected:
    int fd;
    string   filename;
    uint64_t file_size;
    uint64_t chunk_size;

public:
    FileReader(string filename, uint64_t chunk_size = 0) : filename(filename), chunk_size(chunk_size) {
        struct stat sb;

        if (stat(filename.c_str(), &sb) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        // TODO: Try stream reader
        fd = open(filename.c_str(), O_RDONLY);

        // TODO: Check prefered IO block size: https://linux.die.net/man/2/fstat
        file_size = static_cast<uint64_t>(sb.st_size);

        if (chunk_size == 0)
            this->chunk_size = file_size;
    }

    virtual ~FileReader() {};

    virtual unsigned char *read_file() {return nullptr;};

    uint64_t get_chunk_size() const {
        return chunk_size;
    }

    void set_chunk_size(uint64_t chunk_size) {
        this->chunk_size = chunk_size;
    }

    uint64_t getFile_size() const {
        return file_size;
    }

    void setFile_size(uint64_t file_size) {
        FileReader::file_size = file_size;
    }
};


#endif
