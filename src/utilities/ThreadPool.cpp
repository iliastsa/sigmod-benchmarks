#include <unistd.h>
#include <cerrno>
#include <Constants.h>
#include <Utils.h>
#include <cstring>
#include "ThreadPool.h"

ThreadPool::task_queue::task_queue() : head(nullptr), tail(nullptr), n_tasks(0) {
    // TODO: Error checking
    pthread_cond_init(&queue_available, nullptr);
    pthread_mutex_init(&queue_rwlock, nullptr);

    pthread_cond_init(&queue_empty, nullptr);
}

ThreadPool::Task* ThreadPool::task_queue::next() {
    if (n_tasks == 0)
        return nullptr;

    Task* ret = head->task;
    task_queue_node* next = head->next;

    delete head;
    n_tasks--;

    head = next;

    return ret;
}

void ThreadPool::task_queue::insert(Task* task) {
    task_queue_node *new_node = new task_queue_node(task);

    // TODO: Error checking
    pthread_mutex_lock(&queue_rwlock);

    // Insert into queue
    if (n_tasks == 0) {
        head = new_node;
        tail = new_node;
    } else {
        tail->next = new_node;
        tail       = new_node;
    }

    n_tasks++;

    // Signal any thread waiting to read from the queue.
    pthread_cond_signal(&queue_available);

    // Unlock the lock.
    pthread_mutex_unlock(&queue_rwlock);
}

void ThreadPool::task_queue::lock() {
    pthread_mutex_lock(&queue_rwlock);
}

void ThreadPool::task_queue::unlock() {
    pthread_mutex_unlock(&queue_rwlock);
}

void ThreadPool::task_queue::wait() {
    pthread_cond_wait(&queue_available, &queue_rwlock);
}

void ThreadPool::task_queue::wait_empty() {
    pthread_cond_wait(&queue_empty, &queue_rwlock);
}

void ThreadPool::task_queue::broadcast() {
    pthread_cond_broadcast(&queue_available);
}

ThreadPool::task_queue::~task_queue(){
    pthread_mutex_destroy(&queue_rwlock);
    pthread_cond_destroy(&queue_available);
    pthread_cond_destroy(&queue_empty);

    while (head != nullptr) {
        task_queue_node* next = head->next;

        delete head;
        head = next;
    }
}

ThreadPool::ThreadPool(int n_threads, int socket) : pinned(true), socket(socket), running(1), n_threads(n_threads), active(0) {
    threads = new Thread[n_threads];

    // TODO: Error checking

    // Initialize threads
    for (int i = 0; i < n_threads; ++i) {
        threads[i].id = i;
        threads[i].thread_pool = this;

        pthread_create(&threads[i].thread, nullptr, thread_run, threads + i);
    }
}

ThreadPool::ThreadPool(int n_threads) : pinned(false), running(1), n_threads(n_threads), active(0) {
    threads = new Thread[n_threads];

    // TODO: Error checking

    // Initialize threads
    for (int i = 0; i < n_threads; ++i) {
        threads[i].id = i;
        threads[i].thread_pool = this;

        pthread_create(&threads[i].thread, nullptr, thread_run, threads + i);
    }
}

void ThreadPool::add_task(Task* task) {
    task_queue.insert(task);
}


void ThreadPool::pin_current_thread(int core_id) {
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);

    pthread_t current_thread = pthread_self();

    if (int err = pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset))
        P_ERR("Set affinity", err);
}


void* ThreadPool::thread_run(void *thread_arg) {
    Thread *thread = (Thread*) thread_arg;

    int id = thread->id;
    ThreadPool* pool = thread->thread_pool;

    if (pool->pinned) {
        const int cores_per_socket = Constants::N_CORES / Constants::N_SOCKETS;

        const int core = id % (cores_per_socket) + pool->socket * cores_per_socket;
        pin_current_thread(core);
    }



    for (;;) {
        // Acquire lock since the get function requires it
        pool->task_queue.lock();

        while (pool->task_queue.n_tasks == 0 && pool->running == 1)
            pool->task_queue.wait();

        if (pool->running == 0 && pool->task_queue.n_tasks == 0)
            break;

        Task* task = pool->task_queue.next();

        pool->active++;

        pool->task_queue.unlock();

        task->run();

        delete task;

        pool->task_queue.lock();

        pool->active--;

        if (pool->active == 0 && pool->task_queue.n_tasks == 0)
            pool->task_queue.broadcast_queue_empty();

        pool->task_queue.unlock();
    }

    pool->task_queue.unlock();

    pthread_exit(nullptr);

    return nullptr;
}

void ThreadPool::task_queue::broadcast_queue_empty() {
    pthread_cond_broadcast(&queue_empty);
}

void ThreadPool::wait_all() {
    task_queue.lock();

    while (task_queue.n_tasks != 0 || active != 0)
        task_queue.wait_empty();

    task_queue.unlock();
}

ThreadPool::~ThreadPool() {
    task_queue.lock();

    running = 0;

    task_queue.broadcast();

    task_queue.unlock();

    for (int i = 0; i < n_threads; ++i)
        pthread_join(threads[i].thread, nullptr);

    delete[] threads;
}