#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <cstdlib>

class ThreadPool {
public:
    class Task {
    public:
        virtual ~Task() = default;

        virtual void run() = 0;
    };
private:
    struct Thread {
        int id;
        pthread_t thread;

        ThreadPool *thread_pool;
    };

    class task_queue {
    private:
        struct task_queue_node {
            class Task* task;
            task_queue_node *next;

            task_queue_node(class Task* task) : task(task), next(nullptr) {}
        };

        task_queue_node *head;
        task_queue_node *tail;

        // Task queue synchronization;
        pthread_mutex_t queue_rwlock;
        pthread_cond_t  queue_available;

        // Empty queue synchronization;
        pthread_cond_t  queue_empty;

    public:
        task_queue();
        ~task_queue();

        int n_tasks;

        void lock();
        void unlock();
        void wait();
        void wait_empty();
        void broadcast();
        void broadcast_queue_empty();

        void insert(Task* task);
        Task* next();
    } task_queue;

    Thread *threads;

    const bool pinned;
    int socket;

    volatile int running;
    volatile int n_threads;
    volatile int active;

    static void  pin_current_thread(int core_id);
    static void* thread_run(void *t_pool);

public:
    ThreadPool(int n_threads, int core);
    ThreadPool(int n_workers);
    ~ThreadPool();

    void add_task(Task* task);
    void wait_all();
};

#endif