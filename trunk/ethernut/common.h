/*
 * common.h
 *
 * Common functions that have different names in NutOS and POSIX.
 *
 * Use these macros instead of the real function calls.
 *
 * $Id$
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef ETHERNUT
#define MUTEX_INIT(m) NutMutexInit(m)
#define MUTEX_LOCK(m) NutMutexLock(m)
#define MUTEX_UNLOCK(m) NutMutexUnlock(m)
#define MUTEX_DESTROY(m) NutMutexDestroy(m)

#define THREAD_CREATE(name, func, arg, stacksize) NutThreadCreate(name, func, arg, stacksize)
#define THREAD_EXIT() NutThreadExit()

#else

/* POSIX-style threads and mutexes (Linux, FreeBSD, Solaris) */

#define MUTEX_INIT(m) pthread_mutex_init(m, NULL)
#define MUTEX_LOCK(m) log(10, "Locking mutex..."), pthread_mutex_lock(m)
#define MUTEX_UNLOCK(m) log(10, "Unlocking mutex..."), pthread_mutex_unlock(m)
#define MUTEX_DESTROY(m) pthread_mutex_destroy(m)

#define THREAD_CREATE(thread, attr, func, args) pthread_create(thread, attr, func, args)
#define THREAD(func, args) void func(void *args)
#define THREAD_EXIT() pthread_exit(NULL)
typedef pthread_mutex_t MUTEX;
#endif

#endif
