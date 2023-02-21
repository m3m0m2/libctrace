// Copyright (C) 2023, Mauro Meneghin
//
// Trace all function calls

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int log_fd = 2;
static int log_lock = 0;

static int n_threads = 0;
static __thread int thread_use = 0;     // to avoid infinite recursion
static __thread int thread_id = 0;

static void acquire_lock(int* lock)
{
    while (__atomic_exchange_n(lock, 1, __ATOMIC_SEQ_CST) != 0)
            ;
}

static void release_lock(int* lock)
{
    __atomic_exchange_n(lock, 0, __ATOMIC_SEQ_CST);
}

// This static buffer is safe as the use is atomic
#define LOG_BUFFER_SIZE    500
static char log_buffer[LOG_BUFFER_SIZE];

static void log_message(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    acquire_lock(&log_lock);
    vsnprintf(log_buffer, LOG_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    write(log_fd, log_buffer, strlen(log_buffer));
    release_lock(&log_lock);
}

static void log_message_v(const char *fmt, va_list* ap)
{
    acquire_lock(&log_lock);
    vsnprintf(log_buffer, LOG_BUFFER_SIZE, fmt, *ap);
    write(log_fd, log_buffer, strlen(log_buffer));
    release_lock(&log_lock);
}

static void init_thread()
{
    thread_id = __atomic_add_fetch(&n_threads, 1, __ATOMIC_SEQ_CST);
}

static void print_function(const char* type, void* fn)
{
    Dl_info info = {0};

    int rc = dladdr(fn, &info);

    if (rc != 0)
        log_message("%d %s %s [%s]\n", thread_id, type, info.dli_sname, info.dli_fname);
    else
        log_message("%d %s Uknown function at %p\n", thread_id, type, fn);
}

void __cyg_profile_func_enter(void *fn, void *caller)
        __attribute__((no_instrument_function));

void __cyg_profile_func_enter(void *fn, void *caller)
{
    if (thread_id == 0)
        init_thread();
    thread_use++;

    print_function("ENTER", fn);
}

void __cyg_profile_func_exit(void *fn, void *caller)
        __attribute__((no_instrument_function));

void __cyg_profile_func_exit(void *fn, void *caller)
{
    thread_use--;
    print_function("EXIT", fn);
}
