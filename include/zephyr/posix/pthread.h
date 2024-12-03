/*
 * Copyright (c) 2017 Intel Corporation
 * Copyright (c) 2024 Tenstorrent AI ULC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_POSIX_PTHREAD_H_
#define ZEPHYR_INCLUDE_POSIX_PTHREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_POSIX_THREADS)

#include <time.h>

#include <zephyr/posix/sys/types.h>
#include <zephyr/posix/unistd.h>
#include <zephyr/posix/sched.h>

#if defined(_POSIX_BARRIERS)
#define PTHREAD_BARRIER_SERIAL_THREAD -1
#endif
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_CANCEL_ENABLE       0
#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_DISABLE      1
#define PTHREAD_CANCELED            ((void *)-1)
/* PTHREAD_CREATE_DETACHED defined in <sys/_pthreadtypes.h> */
/* PTHREAD_CREATE_JOINABLE defined in <sys/_pthreadtypes.h> */
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
/* PTHREAD_EXPLICIT_SCHED defined in <sys/_pthreadtypes.h> */
/* PTHREAD_INHERIT_SCHED defined in <sys/_pthreadtypes.h> */
#endif
/* PTHREAD_MUTEX_DEFAULT defined in <sys/_pthreadtypes.h> */
/* PTHREAD_MUTEX_ERRORCHECK defined in <sys/_pthreadtypes.h> */
/* PTHREAD_MUTEX_NORMAL defined in <sys/_pthreadtypes.h> */
/* PTHREAD_MUTEX_RECURSIVE defined in <sys/_pthreadtypes.h> */
/* PTHREAD_MUTEX_ROBUST is not defined */
/* PTHREAD_MUTEX_STALLED is not defined */
#define PTHREAD_ONCE_INIT _PTHREAD_ONCE_INIT
#if defined(_POSIX_THREAD_PRIO_PROTECT)
/* PTHREAD_PRIO_INHERIT defined in <sys/_pthreadtypes.h> */
/* PTHREAD_PRIO_NONE defined in <sys/_pthreadtypes.h> */
/* PTHREAD_PRIO_PROTECT defined in <sys/_pthreadtypes.h> */
#endif
#if defined(_POSIX_THREAD_PROCESS_SHARED)
/* PTHREAD_PROCESS_PRIVATE defined in <sys/_pthreadtypes.h> */
/* PTHREAD_PROCESS_SHARED defined in <sys/_pthreadtypes.h> */
#endif
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
/* PTHREAD_SCOPE_PROCESS defined in <sys/_pthreadtypes.h> */
/* PTHREAD_SCOPE_SYSTEM  defined in <sys/_pthreadtypes.h> */
#endif

#define PTHREAD_COND_INITIALIZER   ((pthread_cond_t)_PTHREAD_COND_INITIALIZER)
#define PTHREAD_MUTEX_INITIALIZER  ((pthread_mutex_t)_PTHREAD_MUTEX_INITIALIZER)
#define PTHREAD_RWLOCK_INITIALIZER ((pthread_rwlock_t)_PTHREAD_RWLOCK_INITIALIZER)

/*
 * pthread_attr_t, pthread_barrier_t, pthread_barrierattr_t, pthread_cond_t, pthread_condattr_t,
 * pthread_key_t, pthread_mutex_t, pthread_mutexattr_t, pthread_once_t, pthread_rwlock_t,
 * pthread_rwlockattr_t, pthread_spinlock_t, and pthread_t are defined in <sys/_pthreadtypes.h>
 */

int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);
int pthread_attr_getguardsize(const pthread_attr_t *ZRESTRICT attr, size_t *ZRESTRICT guardsize);
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
int pthread_attr_getinheritsched(const pthread_attr_t *ZRESTRICT attr, int *ZRESTRICT inheritsched);
#endif
int pthread_attr_getschedparam(const pthread_attr_t *ZRESTRICT attr,
			       struct sched_param *ZRESTRICT schedparam);
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
int pthread_attr_getschedpolicy(const pthread_attr_t *ZRESTRICT attr, int *ZRESTRICT policy);
int pthread_attr_getscope(const pthread_attr_t *ZRESTRICT attr, int *ZRESTRICT scope);
#endif
#if defined(_POSIX_THREAD_ATTR_STACKADDR)
int pthread_attr_getstack(const pthread_attr_t *ZRESTRICT attr, void **ZRESTRICT stackaddr,
			  size_t *ZRESTRICT stacksize);
#endif
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
int pthread_attr_getstacksize(const pthread_attr_t *ZRESTRICT attr, size_t *ZRESTRICT stacksize);
#endif
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
#endif
int pthread_attr_setschedparam(pthread_attr_t *ZRESTRICT attr,
			       const struct sched_param *ZRESTRICT schedparam);
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
#endif
#if defined(_POSIX_THREAD_ATTR_STACKADDR)
int pthread_attr_setstack(pthread_attr_t *attr, void *stack, size_t stacksize);
#endif
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
#endif
#if defined(_POSIX_BARRIERS)
int pthread_barrier_destroy(pthread_barrier_t *barrier);
int pthread_barrier_init(pthread_barrier_t *ZRESTRICT barrier,
			 const pthread_barrierattr_t *ZRESTRICT attr, unsigned count);
int pthread_barrier_wait(pthread_barrier_t *barrier);
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *ZRESTRICT barrier,
				   int *ZRESTRICT pshared);
#endif
int pthread_barrierattr_init(pthread_barrierattr_t *attr);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared);
#endif
#endif
int pthread_cancel(pthread_t thread);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_init(pthread_cond_t *ZRESTRICT cond, const pthread_condattr_t *ZRESTRICT att);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_timedwait(pthread_cond_t *ZRESTRICT cond, pthread_mutex_t *ZRESTRICT mutex,
			   const struct timespec *ZRESTRICT abstime);
int pthread_cond_wait(pthread_cond_t *ZRESTRICT cond, pthread_mutex_t *ZRESTRICT mutex);
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_getclock(const pthread_condattr_t *ZRESTRICT attr,
			      clockid_t *ZRESTRICT clock_id);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_condattr_getpshared(const pthread_condattr_t *ZRESTRICT attr, int *ZRESTRICT pshared);
#endif
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
#endif
int pthread_create(pthread_t *ZRESTRICT thread, const pthread_attr_t *ZRESTRICT attr,
		   void *(*start_routine)(void *), void *ZRESTRICT arg);
int pthread_detach(pthread_t thread);
int pthread_equal(pthread_t t1, pthread_t t2);
FUNC_NORETURN void pthread_exit(void *value_ptr);
/* Note: OB - Obsolescent, also which XSI feature test macro? */
int pthread_getconcurrency(void);
#if defined(_POSIX_THREAD_CPUTIME)
int pthread_getcpuclockid(pthread_t thread, clockid_t *clock_id);
#endif
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
int pthread_getschedparam(pthread_t thread, int *ZRESTRICT policy,
			  struct sched_param *ZRESTRICT param);
#endif
void *pthread_getspecific(pthread_key_t key);
int pthread_join(pthread_t thread, void **value_ptr);
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));
int pthread_key_delete(pthread_key_t key);
int pthread_mutex_consistent(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
#if defined(_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutex_getprioceiling(const pthread_mutex_t *ZRESTRICT mutex,
				 int *ZRESTRICT prioceiling);
#endif
int pthread_mutex_init(pthread_mutex_t *ZRESTRICT mutex, const pthread_mutexattr_t *ZRESTRICT attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
#if defined(_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutex_setprioceiling(pthread_mutex_t *ZRESTRICT mutex, int prioceiling,
				 int *ZRESTRICT old_prioceiling);
#endif
int pthread_mutex_timedlock(pthread_mutex_t *ZRESTRICT mutex,
			    const struct timespec *ZRESTRICT abstime);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
#if defined(_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *ZRESTRICT attr,
				     int *ZRESTRICT prioceiling);
#endif
#if defined(_POSIX_THREAD_PRIO_INHERIT) || defined(_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *ZRESTRICT attr,
				  int *ZRESTRICT protocol);
#endif
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *ZRESTRICT attr, int *ZRESTRICT pshared);
#endif
int pthread_mutexattr_getrobust(const pthread_mutexattr_t *ZRESTRICT attr, int *ZRESTRICT robust);
int pthread_mutexattr_gettype(const pthread_mutexattr_t *ZRESTRICT attr, int *ZRESTRICT type);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
#if defined(_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling);
#endif
#if defined(_POSIX_THREAD_PRIO_INHERIT) || defined(_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
#endif
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
#endif
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_init(pthread_rwlock_t *ZRESTRICT rwlock,
			const pthread_rwlockattr_t *ZRESTRICT attr);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *ZRESTRICT rwlock,
			       const struct timespec *ZRESTRICT abstime);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *ZRESTRICT rwlock,
			       const struct timespec *ZRESTRICT abstime);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *ZRESTRICT attr,
				  int *ZRESTRICT pshared);
#endif
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared);
#endif
pthread_t pthread_self(void);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int state, int *oldstate);
/* Note: OB - Obsolescent, also which XSI feature test macro? */
int pthread_setconcurrency(int new_level);
#if defined(_POSIX_THREAD_PROCESS_SHARED)
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
int pthread_setschedprio(pthread_t thread, int prio);
#endif
int pthread_setspecific(pthread_key_t key, const void *value);
#if defined(_POSIX_SPIN_LOCKS)
int pthread_spin_destroy(pthread_spinlock_t *lock);
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
#endif
void pthread_testcancel(void);

void __z_pthread_cleanup_push(void *cleanup[3], void (*routine)(void *arg), void *arg);
void __z_pthread_cleanup_pop(int execute);

#define pthread_cleanup_push(_rtn, _arg)                                                           \
	do /* enforce '{'-like behaviour */ {                                                      \
		void *_z_pthread_cleanup[3];                                                       \
	__z_pthread_cleanup_push(_z_pthread_cleanup, _rtn, _arg)

#define pthread_cleanup_pop(_ex)                                                                   \
	__z_pthread_cleanup_pop(_ex);                                                              \
	} /* enforce '}'-like behaviour */                                                         \
	while (0)

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

#endif

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_POSIX_PTHREAD_H_ */
