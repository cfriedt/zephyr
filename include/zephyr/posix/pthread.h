/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_POSIX_PTHREAD_H_
#define ZEPHYR_INCLUDE_POSIX_PTHREAD_H_

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <zephyr/kernel.h>
#include <sys/features.h>
#include <zephyr/posix/sys/_pthreadtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_POSIX_THREADS) || defined(__DOXYGEN__)

/* Pthread cancellation */
#define PTHREAD_CANCELED       ((void *)-1)
#define PTHREAD_CANCEL_ENABLE  0
#define PTHREAD_CANCEL_DISABLE 1
#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

/* The minimum allowable stack size */
#define PTHREAD_STACK_MIN K_KERNEL_STACK_LEN(0)

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_cond_init(pthread_cond_t *cv, const pthread_condattr_t *att);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_cond_destroy(pthread_cond_t *cv);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_cond_signal(pthread_cond_t *cv);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_cond_broadcast(pthread_cond_t *cv);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_cond_wait(pthread_cond_t *cv, pthread_mutex_t *mut);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_cond_timedwait(pthread_cond_t *cv, pthread_mutex_t *mut,
			   const struct timespec *abstime);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1.
 *
 */
int pthread_condattr_init(pthread_condattr_t *att);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 *
 */
int pthread_condattr_destroy(pthread_condattr_t *att);

/**
 * @brief POSIX threading comatibility API
 *
 * See IEEE 1003.1
 *
 */
int pthread_condattr_getclock(const pthread_condattr_t *ZRESTRICT att,
		clockid_t *ZRESTRICT clock_id);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 *
 */

int pthread_condattr_setclock(pthread_condattr_t *att, clockid_t clock_id);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutex_destroy(pthread_mutex_t *m);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutex_lock(pthread_mutex_t *m);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutex_unlock(pthread_mutex_t *m);

#if defined(_POSIX_TIMEOUTS) || defined(__DOXYGEN__)
/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */

int pthread_mutex_timedlock(pthread_mutex_t *m,
			    const struct timespec *abstime);
#endif

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutex_trylock(pthread_mutex_t *m);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutex_init(pthread_mutex_t *m,
				     const pthread_mutexattr_t *att);

#if defined(_POSIX_THREAD_PRIO_INHERIT) || defined(_POSIX_THREAD_PRIO_PROTECT) ||                  \
	defined(__DOXYGEN__)

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);

/* TODO: mutexattr_getprioceiling / mutexattr_setprioceiling */

#endif

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 *
 * Note that pthread attribute structs are currently noops in Zephyr.
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 *
 * Note that pthread attribute structs are currently noops in Zephyr.
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

#if defined(_POSIX_BARRIERS) || defined(__DOXYGEN__)

#define PTHREAD_BARRIER_SERIAL_THREAD 1

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrier_wait(pthread_barrier_t *b);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrier_init(pthread_barrier_t *b, const pthread_barrierattr_t *attr,
			 unsigned int count);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrier_destroy(pthread_barrier_t *b);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrierattr_init(pthread_barrierattr_t *b);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrierattr_destroy(pthread_barrierattr_t *b);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);

/**
 * @brief POSIX threading compatibility API
 *
 * See IEEE 1003.1
 */
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *ZRESTRICT attr,
				   int *ZRESTRICT pshared);
#endif

/**
 * @brief Obtain ID of the calling thread.
 *
 * The results of calling this API from threads not created with
 * pthread_create() are undefined.
 *
 * See IEEE 1003.1
 */
pthread_t pthread_self(void);

/**
 * @brief Compare thread IDs.
 *
 * See IEEE 1003.1
 */
int pthread_equal(pthread_t pt1, pthread_t pt2);

#if defined(_POSIX_READER_WRITER_LOCKS) || defined(__DOXYGEN__)

int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

/**
 * @brief Destroy the read-write lock attributes object.
 *
 * See IEEE 1003.1
 */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);

/**
 * @brief initialize the read-write lock attributes object.
 *
 * See IEEE 1003.1
 */
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *ZRESTRICT attr,
				  int *ZRESTRICT pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);

#endif

#if defined(_POSIX_THREAD_ATTR_STACKSIZE) || defied(__DOXYGEN__)
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
#endif

int pthread_attr_getguardsize(const pthread_attr_t *ZRESTRICT attr, size_t *ZRESTRICT guardsize);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_getstack(const pthread_attr_t *attr,
			  void **stackaddr, size_t *stacksize);
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr,
			  size_t stacksize);

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) || defined(__DOXYGEN__)
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
#endif

int pthread_once(pthread_once_t *once, void (*initFunc)(void));
FUNC_NORETURN void pthread_exit(void *retval);
int pthread_join(pthread_t thread, void **status);
int pthread_cancel(pthread_t pthread);
int pthread_detach(pthread_t thread);
int pthread_create(pthread_t *newthread, const pthread_attr_t *attr,
		   void *(*threadroutine)(void *), void *arg);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel(void);
int pthread_attr_setschedparam(pthread_attr_t *attr,
			       const struct sched_param *schedparam);
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *schedparam);

#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING) || defined(__DOXYGEN__)
int pthread_getschedparam(pthread_t pthread, int *policy, struct sched_param *param);
int pthread_setschedparam(pthread_t pthread, int policy,
			  const struct sched_param *param);
int pthread_setschedprio(pthread_t thread, int prio);
#endif

int pthread_key_create(pthread_key_t *key,
		void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
int pthread_getconcurrency(void);
int pthread_setconcurrency(int new_level);

void __z_pthread_cleanup_push(void *cleanup[3], void (*routine)(void *arg), void *arg);
void __z_pthread_cleanup_pop(int execute);

#define pthread_cleanup_push(_rtn, _arg)                                                           \
	do /* enforce '{'-like behaviour */ {                                                      \
		void *_z_pthread_cleanup[3];                                                       \
		__z_pthread_cleanup_push(_z_pthread_cleanup, _rtn, _arg)

#define pthread_cleanup_pop(_ex)                                                                   \
		__z_pthread_cleanup_pop(_ex);                                                      \
	} /* enforce '}'-like behaviour */ while (0)

/* Glibc / Oracle Extension Functions */

/**
 * @brief Set name of POSIX thread.
 *
 * Non-portable, extension function that conforms with most
 * other definitions of this function.
 *
 * @param thread POSIX thread to set name
 * @param name Name string
 * @retval 0 Success
 * @retval ESRCH Thread does not exist
 * @retval EINVAL Name buffer is NULL
 * @retval Negative value if kernel function error
 *
 */
int pthread_setname_np(pthread_t thread, const char *name);

/**
 * @brief Get name of POSIX thread and store in name buffer
 *  	  that is of size len.
 *
 * Non-portable, extension function that conforms with most
 * other definitions of this function.
 *
 * @param thread POSIX thread to obtain name information
 * @param name Destination buffer
 * @param len Destination buffer size
 * @retval 0 Success
 * @retval ESRCH Thread does not exist
 * @retval EINVAL Name buffer is NULL
 * @retval Negative value if kernel function error
 */
int pthread_getname_np(pthread_t thread, char *name, size_t len);

#if defined(_POSIX_SPIN_LOCKS) || defined(__DOXYGEN__)
/**
 * @brief Destroy a pthread_spinlock_t.
 *
 * See IEEE 1003.1
 */
int pthread_spin_destroy(pthread_spinlock_t *lock);

/**
 * @brief Initialize a thread_spinlock_t.
 *
 * See IEEE 1003.1
 */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);

/**
 * @brief Lock a previously initialized thread_spinlock_t.
 *
 * See IEEE 1003.1
 */
int pthread_spin_lock(pthread_spinlock_t *lock);

/**
 * @brief Attempt to lock a previously initialized thread_spinlock_t.
 *
 * See IEEE 1003.1
 */
int pthread_spin_trylock(pthread_spinlock_t *lock);

/**
 * @brief Unlock a previously locked thread_spinlock_t.
 *
 * See IEEE 1003.1
 */
int pthread_spin_unlock(pthread_spinlock_t *lock);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_POSIX_PTHREAD_H_ */
