#ifndef SIGMODBENCHMARKS_BENCHMARK_H
#define SIGMODBENCHMARKS_BENCHMARK_H

class Benchmark {
public:
    virtual ~Benchmark() {};

    virtual void run() = 0;
};

#endif
