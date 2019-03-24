#ifndef SIGMODBENCHMARKS_UTILS_H
#define SIGMODBENCHMARKS_UTILS_H

#include <cstdio>

// Useful macros
#if defined(DEBUG)
    #define P_DEBUG(...) std::fprintf(stdout,"[DEBUG] : " __VA_ARGS__)
#else
    #define P_DEBUG(...)
#endif

#define ERR(msg) fprintf(stderr, "%s\n", msg)
#define P_ERR(msg,err) fprintf(stderr, "%s : %s\n", msg, strerror(err))

void f_init(const char *file_name, int *fd, uint64_t *file_size);

#endif
