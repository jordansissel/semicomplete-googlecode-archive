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
#else
#define MUTEX_INIT(m) pthread_mutex_init(m, NULL)
#define MUTEX_LOCK(m) pthread_mutex_lock(m)
#define MUTEX_UNLOCK(m) pthread_mutex_unlock(m)
#define MUTEX_DESTROY(m) pthread_mutex_destroy(m)

#define MUTEX pthread_mutex_t
#endif

#endif
