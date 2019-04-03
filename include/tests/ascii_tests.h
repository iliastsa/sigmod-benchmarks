#ifndef SIGMODBENCHMARKS_ASCII_TESTS_H
#define SIGMODBENCHMARKS_ASCII_TESTS_H

#include <cstdint>
#include <Constants.h>
#include <cassert>
#include <cstdlib>
#include <string>
#include <unordered_set>

bool ascii_key_range(unsigned char* mem, uint64_t file_size) {

    for (uint64_t i = 0; i < file_size; i += Constants::TUPLE_SIZE) {
        for (uint64_t j = 0; j < Constants::KEY_SIZE; j++) {
            if (mem[i + j] < 32 || mem[i + j] > 126)
                return false;
        }
    }

    return true;
}

bool valid_hex(char x) {
    return (x >= '0' && x <= '9') || (x <= 'F' && x >= 'A');
}

bool ascii_payload_form(unsigned char* mem, uint64_t file_size) {

    uint64_t id_num = 0;

    for (uint64_t i = 0; i < file_size; i += Constants::TUPLE_SIZE) {
        uint64_t payload_idx = 10;

        /* 2 spaces after key */
        assert(mem[i + payload_idx] == ' ');
        payload_idx++;
        assert(mem[i + payload_idx] == ' ');
        payload_idx++;


        /* 32 digits long id */

        int id_len = 32;

        /* Extract id to string */
        std::string id(&mem[i + payload_idx], &mem[i + payload_idx + id_len]);

        /* Assert hex chars */
        for (int j = 0; j < id_len; j++, payload_idx++) {
            assert(valid_hex(mem[i + payload_idx]));
        }

        /* Assert successor */
        uint64_t prev_id_num = id_num;
        id_num = std::stoull(id, 0, 16);
        if (i != 0)
            assert(prev_id_num + 1 == id_num);

        /* 2 spaces after id */
        assert(mem[i + payload_idx++] == ' ');
        assert(mem[i + payload_idx++] == ' ');

        /* 52 chars grouped in 4-tuples */

        int tuples_len = 52;

        char base = 0;

        for (int k = 0; k < tuples_len; k++, payload_idx++) {
            if (k % 4 == 0) {
                base = mem[i + payload_idx];
                continue;
            }
            assert(base == mem[i + payload_idx]);
        }
        assert(mem[i + payload_idx++] == '\r');
        assert(mem[i + payload_idx] == '\n');
    }

    return true;
}

void small_payload_unique(unsigned char *mem, uint64_t file_size) {
    std::unordered_set<std::string> set;
    for (uint64_t i = 0; i < file_size; i += Constants::TUPLE_SIZE) {
        uint64_t payload_idx = Constants::KEY_SIZE;
        std::string s(&mem[i + payload_idx], &mem[i + payload_idx + 90]);
        set.insert(s);
    }
    std::cout << "Distinct payloads in small dataset: " << set.size() << std::endl;
}
#endif
