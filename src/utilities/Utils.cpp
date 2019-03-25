#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>

using namespace std;

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
