#ifndef SIGMODBENCHMARKS_ASYNCUTILS_H
#define SIGMODBENCHMARKS_ASYNCUTILS_H

#include <linux/aio_abi.h>

int io_setup(unsigned nr, aio_context_t *ctxp);

int io_destroy(aio_context_t ctx);

int io_submit(aio_context_t ctx, long nr,  struct iocb **iocbpp);

int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
                 struct io_event *events, struct timespec *timeout);

#endif
