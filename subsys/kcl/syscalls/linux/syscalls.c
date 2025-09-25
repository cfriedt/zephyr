#define __NR_getcwd 17
#define __NR_dup 23
#define __NR_fcntl 25
#define __NR_ioctl 29
#define __NR_mknodat 33
#define __NR_mkdirat 34
#define __NR_unlinkat 35
#define __NR_symlinkat 36
#define __NR_linkat 37
#define __NR_truncate 45
#define __NR_ftruncate 46
#define __NR_faccessat 48
#define __NR_chdir 49
#define __NR_fchdir 50
#define __NR_fchmod 52
#define __NR_fchmodat 53
#define __NR_fchownat 54
#define __NR_fchown 55
#define __NR_openat 56
#define __NR_close 57
#define __NR_lseek 62
#define __NR_read 63
#define __NR_write 64
#define __NR_readv 65
#define __NR_writev 66
#define __NR_readlinkat 78
#define __NR_fstat 80
#define __NR_sync 81
#define __NR_fsync 82
#define __NR_fdatasync 83
#define __NR_utimensat 88
#define __NR_exit 93
#define __NR_waitid 95
#define __NR_nanosleep 101
#define __NR_getitimer 102
#define __NR_setitimer 103
#define __NR_timer_create 107
#define __NR_timer_gettime 108
#define __NR_timer_getoverrun 109
#define __NR_timer_settime 110
#define __NR_timer_delete 111
#define __NR_clock_settime 112
#define __NR_clock_gettime 113
#define __NR_clock_getres 114
#define __NR_clock_nanosleep 115
#define __NR_syslog 116
#define __NR_sched_setparam 118
#define __NR_sched_setscheduler 119
#define __NR_sched_getscheduler 120
#define __NR_sched_getparam 121
#define __NR_sched_yield 124
#define __NR_sched_get_priority_max 125
#define __NR_sched_get_priority_min 126
#define __NR_sched_rr_get_interval 127
#define __NR_kill 129
#define __NR_sigaltstack 132
#define __NR_setpriority 140
#define __NR_getpriority 141
#define __NR_setregid 143
#define __NR_setgid 144
#define __NR_setreuid 145
#define __NR_setuid 146
#define __NR_times 153
#define __NR_setpgid 154
#define __NR_getpgid 155
#define __NR_getsid 156
#define __NR_setsid 157
#define __NR_getgroups 158
#define __NR_uname 160
#define __NR_getrlimit 163
#define __NR_setrlimit 164
#define __NR_getrusage 165
#define __NR_umask 166
#define __NR_gettimeofday 169
#define __NR_getpid 172
#define __NR_getppid 173
#define __NR_getuid 174
#define __NR_geteuid 175
#define __NR_getgid 176
#define __NR_getegid 177
#define __NR_mq_open 180
#define __NR_mq_unlink 181
#define __NR_mq_timedsend 182
#define __NR_mq_timedreceive 183
#define __NR_mq_notify 184
#define __NR_msgget 186
#define __NR_msgctl 187
#define __NR_msgrcv 188
#define __NR_msgsnd 189
#define __NR_semget 190
#define __NR_semctl 191
#define __NR_semop 193
#define __NR_shmget 194
#define __NR_shmctl 195
#define __NR_shmat 196
#define __NR_shmdt 197
#define __NR_socket 198
#define __NR_socketpair 199
#define __NR_bind 200
#define __NR_listen 201
#define __NR_accept 202
#define __NR_connect 203
#define __NR_getsockname 204
#define __NR_getpeername 205
#define __NR_sendto 206
#define __NR_recvfrom 207
#define __NR_setsockopt 208
#define __NR_getsockopt 209
#define __NR_shutdown 210
#define __NR_sendmsg 211
#define __NR_recvmsg 212
#define __NR_munmap 215
#define __NR_execve 221
#define __NR_mmap 222
#define __NR_mprotect 226
#define __NR_msync 227
#define __NR_mlock 228
#define __NR_munlock 229
#define __NR_mlockall 230
#define __NR_munlockall 231
typedef long (*syscall_t)();
syscall_t getcwd;
syscall_t dup;
syscall_t fcntl;
syscall_t ioctl;
syscall_t mknodat;
syscall_t mkdirat;
syscall_t unlinkat;
syscall_t symlinkat;
syscall_t linkat;
syscall_t truncate;
syscall_t ftruncate;
syscall_t faccessat;
syscall_t chdir;
syscall_t fchdir;
syscall_t fchmod;
syscall_t fchmodat;
syscall_t fchownat;
syscall_t fchown;
syscall_t openat;
syscall_t close;
syscall_t lseek;
syscall_t read;
syscall_t write;
syscall_t readv;
syscall_t writev;
syscall_t readlinkat;
syscall_t fstat;
syscall_t sync;
syscall_t fsync;
syscall_t fdatasync;
syscall_t utimensat;
syscall_t exit;
syscall_t waitid;
syscall_t nanosleep;
syscall_t getitimer;
syscall_t setitimer;
syscall_t timer_create;
syscall_t timer_gettime;
syscall_t timer_getoverrun;
syscall_t timer_settime;
syscall_t timer_delete;
syscall_t clock_settime;
syscall_t clock_gettime;
syscall_t clock_getres;
syscall_t clock_nanosleep;
syscall_t syslog;
syscall_t sched_setparam;
syscall_t sched_setscheduler;
syscall_t sched_getscheduler;
syscall_t sched_getparam;
syscall_t sched_yield;
syscall_t sched_get_priority_max;
syscall_t sched_get_priority_min;
syscall_t sched_rr_get_interval;
syscall_t kill;
syscall_t sigaltstack;
syscall_t setpriority;
syscall_t getpriority;
syscall_t setregid;
syscall_t setgid;
syscall_t setreuid;
syscall_t setuid;
syscall_t times;
syscall_t setpgid;
syscall_t getpgid;
syscall_t getsid;
syscall_t setsid;
syscall_t getgroups;
syscall_t uname;
syscall_t getrlimit;
syscall_t setrlimit;
syscall_t getrusage;
syscall_t umask;
syscall_t gettimeofday;
syscall_t getpid;
syscall_t getppid;
syscall_t getuid;
syscall_t geteuid;
syscall_t getgid;
syscall_t getegid;
syscall_t mq_open;
syscall_t mq_unlink;
syscall_t mq_timedsend;
syscall_t mq_timedreceive;
syscall_t mq_notify;
syscall_t msgget;
syscall_t msgctl;
syscall_t msgrcv;
syscall_t msgsnd;
syscall_t semget;
syscall_t semctl;
syscall_t semop;
syscall_t shmget;
syscall_t shmctl;
syscall_t shmat;
syscall_t shmdt;
syscall_t socket;
syscall_t socketpair;
syscall_t bind;
syscall_t listen;
syscall_t accept;
syscall_t connect;
syscall_t getsockname;
syscall_t getpeername;
syscall_t sendto;
syscall_t recvfrom;
syscall_t setsockopt;
syscall_t getsockopt;
syscall_t shutdown;
syscall_t sendmsg;
syscall_t recvmsg;
syscall_t munmap;
syscall_t execve;
syscall_t mmap;
syscall_t mprotect;
syscall_t msync;
syscall_t mlock;
syscall_t munlock;
syscall_t mlockall;
syscall_t munlockall;
const syscall_t syscalls[] = {
[__NR_getcwd] = getcwd,
[__NR_dup] = dup,
[__NR_fcntl] = fcntl,
[__NR_ioctl] = ioctl,
[__NR_mknodat] = mknodat,
[__NR_mkdirat] = mkdirat,
[__NR_unlinkat] = unlinkat,
[__NR_symlinkat] = symlinkat,
[__NR_linkat] = linkat,
[__NR_truncate] = truncate,
[__NR_ftruncate] = ftruncate,
[__NR_faccessat] = faccessat,
[__NR_chdir] = chdir,
[__NR_fchdir] = fchdir,
[__NR_fchmod] = fchmod,
[__NR_fchmodat] = fchmodat,
[__NR_fchownat] = fchownat,
[__NR_fchown] = fchown,
[__NR_openat] = openat,
[__NR_close] = close,
[__NR_lseek] = lseek,
[__NR_read] = read,
[__NR_write] = write,
[__NR_readv] = readv,
[__NR_writev] = writev,
[__NR_readlinkat] = readlinkat,
[__NR_fstat] = fstat,
[__NR_sync] = sync,
[__NR_fsync] = fsync,
[__NR_fdatasync] = fdatasync,
[__NR_utimensat] = utimensat,
[__NR_exit] = exit,
[__NR_waitid] = waitid,
[__NR_nanosleep] = nanosleep,
[__NR_getitimer] = getitimer,
[__NR_setitimer] = setitimer,
[__NR_timer_create] = timer_create,
[__NR_timer_gettime] = timer_gettime,
[__NR_timer_getoverrun] = timer_getoverrun,
[__NR_timer_settime] = timer_settime,
[__NR_timer_delete] = timer_delete,
[__NR_clock_settime] = clock_settime,
[__NR_clock_gettime] = clock_gettime,
[__NR_clock_getres] = clock_getres,
[__NR_clock_nanosleep] = clock_nanosleep,
[__NR_syslog] = syslog,
[__NR_sched_setparam] = sched_setparam,
[__NR_sched_setscheduler] = sched_setscheduler,
[__NR_sched_getscheduler] = sched_getscheduler,
[__NR_sched_getparam] = sched_getparam,
[__NR_sched_yield] = sched_yield,
[__NR_sched_get_priority_max] = sched_get_priority_max,
[__NR_sched_get_priority_min] = sched_get_priority_min,
[__NR_sched_rr_get_interval] = sched_rr_get_interval,
[__NR_kill] = kill,
[__NR_sigaltstack] = sigaltstack,
[__NR_setpriority] = setpriority,
[__NR_getpriority] = getpriority,
[__NR_setregid] = setregid,
[__NR_setgid] = setgid,
[__NR_setreuid] = setreuid,
[__NR_setuid] = setuid,
[__NR_times] = times,
[__NR_setpgid] = setpgid,
[__NR_getpgid] = getpgid,
[__NR_getsid] = getsid,
[__NR_setsid] = setsid,
[__NR_getgroups] = getgroups,
[__NR_uname] = uname,
[__NR_getrlimit] = getrlimit,
[__NR_setrlimit] = setrlimit,
[__NR_getrusage] = getrusage,
[__NR_umask] = umask,
[__NR_gettimeofday] = gettimeofday,
[__NR_getpid] = getpid,
[__NR_getppid] = getppid,
[__NR_getuid] = getuid,
[__NR_geteuid] = geteuid,
[__NR_getgid] = getgid,
[__NR_getegid] = getegid,
[__NR_mq_open] = mq_open,
[__NR_mq_unlink] = mq_unlink,
[__NR_mq_timedsend] = mq_timedsend,
[__NR_mq_timedreceive] = mq_timedreceive,
[__NR_mq_notify] = mq_notify,
[__NR_msgget] = msgget,
[__NR_msgctl] = msgctl,
[__NR_msgrcv] = msgrcv,
[__NR_msgsnd] = msgsnd,
[__NR_semget] = semget,
[__NR_semctl] = semctl,
[__NR_semop] = semop,
[__NR_shmget] = shmget,
[__NR_shmctl] = shmctl,
[__NR_shmat] = shmat,
[__NR_shmdt] = shmdt,
[__NR_socket] = socket,
[__NR_socketpair] = socketpair,
[__NR_bind] = bind,
[__NR_listen] = listen,
[__NR_accept] = accept,
[__NR_connect] = connect,
[__NR_getsockname] = getsockname,
[__NR_getpeername] = getpeername,
[__NR_sendto] = sendto,
[__NR_recvfrom] = recvfrom,
[__NR_setsockopt] = setsockopt,
[__NR_getsockopt] = getsockopt,
[__NR_shutdown] = shutdown,
[__NR_sendmsg] = sendmsg,
[__NR_recvmsg] = recvmsg,
[__NR_munmap] = munmap,
[__NR_execve] = execve,
[__NR_mmap] = mmap,
[__NR_mprotect] = mprotect,
[__NR_msync] = msync,
[__NR_mlock] = mlock,
[__NR_munlock] = munlock,
[__NR_mlockall] = mlockall,
[__NR_munlockall] = munlockall,
};
