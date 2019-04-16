#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>
#include <Constants.h>
#include <Utils.h>

using namespace std;



uint64_t get_file_size(const char *file_name) {
    struct stat sb = {0};

    if (stat(file_name, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    return static_cast<uint64_t>(sb.st_size);
}

Constants::Dataset determine_dataset(const char *file_name) {
    uint64_t file_size = get_file_size(file_name);

    if (file_size == GB(10))
        return Constants::Dataset::SMALL;
    else if (file_size == GB(20))
        return Constants::Dataset::MEDIUM;
    else if (file_size == GB(60))
        return Constants::Dataset::LARGE;
    else
        return Constants::Dataset::UNDEFINED;
}

void f_init(const char *file_name, int *fd, uint64_t *file_size) {
    struct stat sb = {0};

    if (stat(file_name, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    cout << "Preferred block size: " << sb.st_blksize << endl;

    // TODO: Check prefered IO block size: https://linux.die.net/man/2/fstat
    *fd = open(file_name, O_RDONLY);
    *file_size = static_cast<uint64_t>(sb.st_size);

    // TODO: Check if the command below has any performance impact
    // posix_fadvise(*fd, 0, *file_size, POSIX_FADV_SEQUENTIAL);
}
