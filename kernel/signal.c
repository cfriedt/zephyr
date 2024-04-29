#include <zephyr/kernel.h>
#include <zephyr/internal/syscall_handler.h>

int k_sigaddset(struct k_sigset *set, int sig)
{
	int bit = (sig - 1) % BITS_PER_LONG;
	int i = (sig - 1) / BITS_PER_LONG;

	if (set == NULL || sig <= 0 || sig >= K_NSIG) {
		return -EINVAL;
	}

	set->sig[i] |= BIT(bit);

	return 0;
}

int k_sigdelset(struct k_sigset *set, int sig)
{
	int bit = (sig - 1) % BITS_PER_LONG;
	int i = (sig - 1) / BITS_PER_LONG;

	if (set == NULL || sig <= 0 || sig >= K_NSIG) {
		return -EINVAL;
	}

	set->sig[i] &= ~BIT(bit);

	return 0;
}

int k_sigismember(struct k_sigset *set, int sig)
{
	int bit = (sig - 1) % BITS_PER_LONG;
	int i = (sig - 1) / BITS_PER_LONG;

	if (set == NULL || sig <= 0 || sig >= K_NSIG) {
		return -EINVAL;
	}

	return (set->sig[i] & BIT(bit)) != 0;
}

int k_sigfillset(struct k_sigset *set)
{
	if (set == NULL) {
		return -EINVAL;
	}

	for (size_t i = 0; i < ARRAY_SIZE(set->sig); ++i) {
		set->sig[i] = -1;
	}

	return 0;
}

int k_sigemptyset(struct k_sigset *set)
{
	if (set == NULL) {
		return -EINVAL;
	}

	for (size_t i = 0; i < ARRAY_SIZE(set->sig); ++i) {
		set->sig[i] = 0;
	}

	return 0;
}

struct k_sig_and_action {
	int sig;
	struct k_signal_action act;
	sys_dnode_t head;
};

static struct k_spinlock sig_lock;
K_MEM_SLAB_DEFINE_STATIC(sigact_slab, sizeof(struct k_sig_and_action), CONFIG_SIGNAL_HANDLERS_MAX,
			 __alignof(struct k_sig_and_action));

static struct k_sig_and_action *sigact_alloc(void)
{
	struct k_sig_and_action *ent = NULL;

	if (k_mem_slab_alloc(&sigact_slab, (void **)&ent, K_NO_WAIT) < 0) {
		return NULL;
	}

	__ASSERT_NO_MSG(ent != NULL);
	sys_dlist_init(&ent->head);

	return ent;
}

static void sigact_free(struct k_sig_and_action *ent)
{
	k_mem_slab_free(&sigact_slab, ent);
}

static struct k_sig_and_action *sigaction_find(struct k_thread *th, int sig)
{
	struct k_sig_and_action *it;

	SYS_DLIST_FOR_EACH_CONTAINER(&th->sigactions, it, head) {
		if (it->sig == sig) {
			return it;
		}
	}

	return NULL;
}

K_MEM_SLAB_DEFINE_STATIC(sig_slab, sizeof(struct k_queued_signal), CONFIG_QUEUED_SIGNALS_MAX,
			 __alignof(struct k_queued_signal));

static struct k_queued_signal *sig_alloc(void)
{
	struct k_queued_signal *ent = NULL;

	if (k_mem_slab_alloc(&sig_slab, (void **)&ent, K_NO_WAIT) < 0) {
		return NULL;
	}

	__ASSERT_NO_MSG(ent != NULL);
	sys_dlist_init(&ent->head);

	return ent;
}

static void sig_free(struct k_queued_signal *ent)
{
	k_mem_slab_free(&sig_slab, ent);
}

void z_check_signals(struct k_thread *thread)
{
	struct k_queued_signal *qsig;
	struct k_sig_and_action *act = NULL;
	struct k_queued_signal *found = NULL;

	if (thread == NULL) {
		return;
	}

	K_SPINLOCK(&sig_lock) {
		SYS_DLIST_FOR_EACH_CONTAINER(&thread->sigqueue, qsig, head) {
			if (k_sigismember(&thread->sigmask, qsig->signal)) {
				continue;
			}

			found = qsig;
			break;
		}

		if (found == NULL) {
			K_SPINLOCK_BREAK;
		}

		sys_dlist_remove(&found->head);
		printk("Found signal %d\n", found->signal);

		SYS_DLIST_FOR_EACH_CONTAINER(&thread->sigactions, act, head) {
			if (act->sig == found->signal) {
				break;
			}
		}
	}

	if (act != NULL) {
		__ASSERT_NO_MSG(found != NULL);
		printk("executing action %p\n", act->act.sa_handler);
		act->act.sa_handler(found->signal);
	}

	if (found != NULL) {
		sig_free(found);
	}
}

int z_impl_k_sigaction(int sig, const struct k_signal_action *ZRESTRICT act,
		       struct k_signal_action *ZRESTRICT oact)
{
	int ret = 0;
	struct k_thread *self = _current;

	if (sig < 0 || sig >= K_NSIG) {
		return -EINVAL;
	}

	if (sig == 0) {
		return 0;
	}

	K_SPINLOCK(&sig_lock) {
		struct k_sig_and_action *sa = sigaction_find(self, sig);

		if (oact != NULL && sa != NULL) {
			*oact = sa->act;
		}

		if (act == NULL) {
			K_SPINLOCK_BREAK;
		}

		if (act == K_SIGACTION_REMOVE) {
			if (sa != NULL) {
				sys_dlist_remove(&sa->head);
				sigact_free(sa);
			}
			K_SPINLOCK_BREAK;
		}

		if (sa != NULL) {
			*sa = (struct k_sig_and_action){
				.sig = sig,
				.act = *act,
			};
			K_SPINLOCK_BREAK;
		}

		sa = sigact_alloc();
		if (sa == NULL) {
			ret = -ENOMEM;
			K_SPINLOCK_BREAK;
		}

		*sa = (struct k_sig_and_action){
			.sig = sig,
			.act = *act,
		};
		sys_dlist_append(&self->sigactions, &sa->head);
	}

	return ret;
}

int z_impl_k_sigaltstack(const struct k_signal_stack *ZRESTRICT ss,
			 struct k_signal_stack *ZRESTRICT old_ss)
{
	int ret = 0;

	K_SPINLOCK(&sig_lock) {
		if (old_ss != NULL) {
			*old_ss = _current->sigaltstack;
		}

		if (ss == NULL) {
			K_SPINLOCK_BREAK;
		}

		if (_current->sigaltstack.ss_flags == K_SIGALTSTACK_ONSTACK) {
			ret = -EPERM;
			K_SPINLOCK_BREAK;
		}

		if (ss == K_SIGALTSTACK_REMOVE) {
			_current->sigaltstack = (struct k_signal_stack){0};
			K_SPINLOCK_BREAK;
		}

		if (ss->ss_flags != 0) {
			ret = -EINVAL;
			K_SPINLOCK_BREAK;
		}

		if (ss->ss_size < K_KERNEL_STACK_LEN(0)) {
			ret = -ENOMEM;
			K_SPINLOCK_BREAK;
		}

		_current->sigaltstack = *ss;
	}

	return ret;
}

int z_impl_k_sigmask(int how, const struct k_sigset *ZRESTRICT set, struct k_sigset *ZRESTRICT oset)
{
	int ret = 0;
	struct k_thread *t = _current;

	if (!(how == K_SIGBLOCK || how == K_SIGSETMASK || how == K_SIGUNBLOCK)) {
		return -EINVAL;
	}

	K_SPINLOCK(&sig_lock) {
		if (oset != NULL) {
			*oset = t->sigmask;
		}

		if (set == NULL) {
			K_SPINLOCK_BREAK;
		}

		switch (how) {
		case K_SIGBLOCK:
			for (size_t i = 0; i < ARRAY_SIZE(set->sig); ++i) {
				t->sigmask.sig[i] |= set->sig[i];
			}
			break;
		case K_SIGSETMASK:
			t->sigmask = *set;
			break;
		case K_SIGUNBLOCK:
			for (size_t i = 0; i < ARRAY_SIZE(set->sig); ++i) {
				t->sigmask.sig[i] &= ~set->sig[i];
			}
			break;
		}
	}

	return ret;
}

int z_impl_k_thread_kill(k_tid_t thread, int sig)
{
	int ret = 0;
	struct k_queued_signal *qsig;

	if (thread == NULL) {
		return -EINVAL;
	}

	if (sig < 0 || sig >= K_NSIG) {
		return -EINVAL;
	}

	if (sig == 0) {
		return 0;
	}

	K_SPINLOCK(&sig_lock) {
		qsig = sig_alloc();
		if (qsig == NULL) {
			ret = -ENOMEM;
			K_SPINLOCK_BREAK;
		}

		qsig->signal = sig;
		sys_dlist_append(&thread->sigqueue, &qsig->head);
	}

	return ret;
}

int z_impl_k_sigpending(struct k_sigset *set)
{
	int ret = 0;
	struct k_queued_signal *qsig;
	struct k_thread *t = _current;

	K_SPINLOCK(&sig_lock) {
		if (set == NULL) {
			ret = -EFAULT;
			K_SPINLOCK_BREAK;
		}

		k_sigemptyset(set);
		SYS_DLIST_FOR_EACH_CONTAINER(&t->sigqueue, qsig, head) {
			k_sigaddset(set, qsig->signal);
		}
	}

	return ret;
}

int z_impl_k_sigsuspend(struct k_sigset *set)
{
	ARG_UNUSED(set);

	return -ENOSYS;
}

int z_impl_k_sigtimedwait(const struct k_sigset *ZRESTRICT set, struct k_siginfo *ZRESTRICT info,
			  const k_timeout_t *ZRESTRICT timeout)
{
	ARG_UNUSED(set);
	ARG_UNUSED(info);
	ARG_UNUSED(timeout);

	return -ENOSYS;
}

#ifdef CONFIG_USERSPACE

static inline int z_vrfy_k_sigaction(int sig, const struct k_signal_action *ZRESTRICT act,
				     struct k_signal_action *ZRESTRICT oact)
{
	if (!(act == NULL || act == K_SIGACTION_REMOVE ||
	      K_SYSCALL_MEMORY_READ(act, sizeof(*act)))) {
		return -EFAULT;
	}

	if (!(oact == NULL || K_SYSCALL_MEMORY_WRITE(oact, sizeof(*oact)))) {
		return -EFAULT;
	}

	return z_impl_k_sigaction(sig, act, oact);
}
#include <syscalls/k_sigaction_mrsh.c>

static inline int z_vrfy_k_sigmask(int how, const struct k_sigset *ZRESTRICT set,
				   struct k_sigset *ZRESTRICT oset)
{
	if (set != NULL) {
		K_OOPS(K_SYSCALL_MEMORY_READ(set, sizeof(*set)));
	}

	if (oset != NULL) {
		K_OOPS(K_SYSCALL_MEMORY_WRITE(oset, sizeof(*oset)));
	}

	return z_impl_k_sigmask(how, set, oset);
}
#include <syscalls/k_sigmask_mrsh.c>

static inline int z_vrfy_k_thread_kill(k_tid_t thread, int sig)
{
	int ret = k_object_validate(k_object_find(thread), K_OBJ_THREAD, _OBJ_INIT_TRUE);

	switch (ret) {
	case 0:
		break;
	case -EPERM:
		return -EPERM;
	default:
		return -EINVAL;
	}

	return z_impl_k_thread_kill(thread, sig);
}
#include <syscalls/k_thread_kill_mrsh.c>

static inline int z_vrfy_k_sigaltstack(const struct k_signal_stack *ZRESTRICT ss,
				       struct k_signal_stack *ZRESTRICT old_ss)
{
	if (!(ss == NULL || ss == K_SIGALTSTACK_REMOVE || K_SYSCALL_MEMORY_READ(ss, sizeof(*ss)))) {
		return -EFAULT;
	}

	if (!(old_ss == NULL || K_SYSCALL_MEMORY_WRITE(old_ss, sizeof(*old_ss)))) {
		return -EFAULT;
	}

	return z_impl_k_sigaltstack(ss, old_ss);
}
#include <syscalls/k_sigaltstack_mrsh.c>

static inline int z_vrfy_k_sigpending(struct k_sigset *set)
{
	if (set == NULL || !K_SYSCALL_MEMORY_WRITE(set, sizeof(*set))) {
		return -EFAULT;
	}

	return z_impl_k_sigpending(set);
}
#include <syscalls/k_sigpending_mrsh.c>

#endif /* CONFIG_USERSPACE */
