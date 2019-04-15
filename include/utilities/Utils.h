#ifndef SIGMODBENCHMARKS_UTILS_H
#define SIGMODBENCHMARKS_UTILS_H

#include <cstdio>
#include <cstdint>

#include "Constants.h"

// Useful macros
#if defined(DEBUG)
    #define P_DEBUG(...) std::fprintf(stdout,"[DEBUG] : " __VA_ARGS__)
#else
    #define P_DEBUG(...)
#endif

#define ERR(msg) fprintf(stdout, "%s\n", msg)
#define P_ERR(msg,err) fprintf(stdout, "%s : %s\n", msg, strerror(err))

#define PAGE_ADDR(addr) (void *)(((uintptr_t)(addr)) & (uint64_t)(~(4096ULL - 1ULL)))

#define RUN_WHEN(DATASET, CODE) {if (Constants::dataset == DATASET) { CODE }}

void f_init(const char *file_name, int *fd, uint64_t *file_size);



#endif
