
#include "utils.h"
#include "Timer.h"
#include "Constants.h"


int main(int argc, char **argv){
    uint32_t fd;
    uint64_t file_size;
    struct stat sb = {0};
    fd = open(argv[1], O_RDONLY);

    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    file_size = static_cast<uint64_t>(sb.st_size);
    cout << "file size: " << file_size << endl;
    uint32_t num_tuples = file_size / 100;
    cout << "Tuples: " << num_tuples << endl;
    Tuple *tuples = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));
    for (uint32_t i = 0; i < num_tuples; i++) {
        tuples[i].key = static_cast<unsigned char*>(malloc(Constants::KEY_SIZE * sizeof(char)));
        // tuples[i].payload = static_cast<unsigned char*>(malloc(Constanst::PAYLOAD * sizeof(char)));
    }
    Timer timer;

    timer.run();

    readFile(num_tuples, tuples, fd, file_size);

    timer.stop();

    cout << "Column store time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;

    timer.run();
    // radixsort(tuples, num_tuples);
    radixsort2(tuples, num_tuples);
    timer.stop();

    cout << "RadixSort time: " << std::fixed << timer.elapsedMilliseconds() << " ms" << endl;
    
    for (uint32_t i = 0; i < num_tuples; i++) {
        // free(tuples[i].payload);
        free(tuples[i].key);
    }
    free(tuples);
    close(fd);
    return 0;
}
