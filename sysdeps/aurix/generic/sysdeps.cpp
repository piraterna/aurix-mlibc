#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/vm-flags.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <termios.h>

#define SYS_EXIT 0
#define SYS_OPEN 1
#define SYS_READ 2
#define SYS_WRITE 3
#define SYS_CLOSE 4
#define SYS_IOCTL 6
#define SYS_MMAP 9
#define SYS_LSEEK 10
#define SYS_MUNMAP 11
#define SYS_CLOCK_GET 12
#define SYS_SET_FS_BASE 13
#define SYS_MPROTECT 14
#define SYS_GETCWD 15
#define SYS_FORK 16
#define SYS_CHDIR 17
#define SYS_WAITPID 18
#define SYS_EXECVE 19
#define SYS_OPENDIR 20
#define SYS_READENTRIES 21
#define SYS_STAT 22
#define SYS_GETPID 23
#define SYS_GETUID 24
#define SYS_GETEUID 25
#define SYS_GETGID 26
#define SYS_GETEGID 27
#define SYS_GETPPID 28
#define SYS_GETTID 29
#define SYS_FCNTL 30
#define SYS_OPENAT 31
#define SYS_READLINK 32
#define SYS_POLL 33
#define SYS_DUP 34
#define SYS_DUP2 35
#define SYS_DUP3 36
#define SYS_PIPE 37
#define SYS_PIPE2 38
#define SYS_MKDIR 39
#define SYS_MKDIRAT 40
#define SYS_UNLINKAT 41
#define SYS_RENAME 42
#define SYS_SYMLINK 43
#define SYS_SYMLINKAT 44
#define SYS_LINK 45
#define SYS_LINKAT 46
#define SYS_CHMOD 47
#define SYS_UMASK 48
#define SYS_KILL 49
#define SYS_SLEEP 50
#define SYS_SIGACTION 51
#define SYS_FACCESSAT 52
#define SYS_UTIMENSAT 53
#define SYS_GETPGID 54

#ifndef TCGETS
#define TCGETS 0x5401
#endif
#ifndef TCSETS
#define TCSETS 0x5402
#endif
#ifndef TCSETSW
#define TCSETSW 0x5403
#endif
#ifndef TCSETSF
#define TCSETSF 0x5404
#endif
#ifndef TCSBRK
#define TCSBRK 0x5409
#endif
#ifndef TCXONC
#define TCXONC 0x540A
#endif
#ifndef TCFLSH
#define TCFLSH 0x540B
#endif
#ifndef TIOCGWINSZ
#define TIOCGWINSZ 0x5413
#endif
#ifndef TIOCSWINSZ
#define TIOCSWINSZ 0x5414
#endif

#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif

namespace {
inline int sc_error(long ret) { return ret < 0 ? -ret : 0; }
inline void copy_uts_field(char *dst, size_t dst_size, const char *src) {
	if (!dst_size)
		return;
	size_t len = strlen(src);
	if (len >= dst_size)
		len = dst_size - 1;
	memcpy(dst, src, len);
	dst[len] = '\0';
}
} // namespace

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	ssize_t _ignored = 0;
	sysdep<Write>(1, msg, strlen(msg), &_ignored);
	sysdep<Write>(1, "\n", 1, &_ignored);
}

int Sysdeps<Isatty>::operator()(int fd) {
	struct termios attr;
	auto sc_ret = syscall(SYS_IOCTL, fd, TCGETS, &attr);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Write>::operator()(int fd, void const *buf, size_t size, ssize_t *ret) {
	auto sc_ret = syscall(SYS_WRITE, fd, buf, size);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (ret)
		*ret = static_cast<ssize_t>(sc_ret);
	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	auto sc_ret = syscall(SYS_SET_FS_BASE, pointer);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	auto sc_ret = syscall(
	    SYS_MMAP, nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
	);
	if (int e = sc_error(sc_ret); e)
		return e;
	*pointer = reinterpret_cast<void *>(sc_ret);
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *pointer, unsigned long size) {
	return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	auto sc_ret = syscall(SYS_LSEEK, fd, offset, whence);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (new_offset)
		*new_offset = static_cast<off_t>(sc_ret);
	return 0;
}

void Sysdeps<Exit>::operator()(int status) {
	syscall(SYS_EXIT, status);
	__builtin_unreachable();
}

int Sysdeps<Close>::operator()(int fd) {
	auto sc_ret = syscall(SYS_CLOSE, fd);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<FutexWake>::operator()(int *, bool) { return 0; }
int Sysdeps<FutexWait>::operator()(int *pointer, int expected, timespec const *) {
	if (__atomic_load_n(pointer, __ATOMIC_RELAXED) != expected)
		return EAGAIN;
	return ENOSYS;
}

int Sysdeps<Read>::operator()(int fd, void *buf, unsigned long size, long *ret) {
	auto sc_ret = syscall(SYS_READ, fd, buf, size);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (ret)
		*ret = sc_ret;
	return 0;
}

int Sysdeps<Open>::operator()(const char *path, int flags, unsigned int mode, int *fd) {
	auto sc_ret = syscall(SYS_OPEN, path, flags, mode);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (fd)
		*fd = static_cast<int>(sc_ret);
	return 0;
}

int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	auto sc_ret = syscall(SYS_OPENAT, dirfd, path, flags, mode);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (fd)
		*fd = static_cast<int>(sc_ret);
	return 0;
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
	auto sc_ret = syscall(SYS_OPENDIR, path, handle);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int
Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	auto sc_ret = syscall(SYS_READENTRIES, handle, buffer, max_size, bytes_read);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<VmMap>::operator()(
    void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window
) {
	auto sc_ret = syscall(SYS_MMAP, hint, size, prot, flags, fd, offset);
	if (int e = sc_error(sc_ret); e)
		return e;
	*window = reinterpret_cast<void *>(sc_ret);
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	auto sc_ret = syscall(SYS_MUNMAP, pointer, size);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	auto sc_ret = syscall(SYS_MPROTECT, pointer, size, prot);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	auto sc_ret = syscall(SYS_CLOCK_GET, clock, secs, nanos);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	if (*nanos >= 1000000000L)
		return EINVAL;
	auto sc_ret = syscall(SYS_SLEEP, *secs, *nanos);
	if (int e = sc_error(sc_ret); e)
		return e;
	*secs = 0;
	*nanos = 0;
	return 0;
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	auto sc_ret = syscall(SYS_GETCWD, buffer, size);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
	struct utsname name;
	int i = sysdep<Uname>(&name);
	if (i)
		return i;
	if (bufsize >= sizeof(name.nodename))
		bufsize = sizeof(name.nodename) - 1;
	memcpy(buffer, name.nodename, bufsize);
	return 0;
}

int Sysdeps<Uname>::operator()(struct utsname *buf) {
	copy_uts_field(buf->sysname, sizeof(buf->sysname), "Aurix");
	copy_uts_field(buf->nodename, sizeof(buf->nodename), "aurix");
	copy_uts_field(buf->release, sizeof(buf->release), "0.0.0");
	copy_uts_field(buf->version, sizeof(buf->version), "aurix");
#if defined(__x86_64__)
	copy_uts_field(buf->machine, sizeof(buf->machine), "x86_64");
#elif defined(__aarch64__)
	copy_uts_field(buf->machine, sizeof(buf->machine), "aarch64");
#elif defined(__riscv)
	copy_uts_field(buf->machine, sizeof(buf->machine), "riscv");
#else
	copy_uts_field(buf->machine, sizeof(buf->machine), "unknown");
#endif
	return 0;
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	auto sc_ret = syscall(SYS_FORK);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (child)
		*child = static_cast<pid_t>(sc_ret);
	return 0;
}

int Sysdeps<Chdir>::operator()(const char *path) {
	auto sc_ret = syscall(SYS_CHDIR, path);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int
Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *, pid_t *ret_pid) {
	auto sc_ret = syscall(SYS_WAITPID, pid, status, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (ret_pid)
		*ret_pid = static_cast<pid_t>(sc_ret);
	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	auto sc_ret = syscall(SYS_EXECVE, path, argv, envp);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	auto sc_ret = syscall(SYS_MKDIR, path, mode);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	auto sc_ret = syscall(SYS_MKDIRAT, dirfd, path, mode);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Unlinkat>::operator()(int dirfd, const char *path, int flags) {
	auto sc_ret = syscall(SYS_UNLINKAT, dirfd, path, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Rmdir>::operator()(const char *path) {
	return sysdep<Unlinkat>(AT_FDCWD, path, AT_REMOVEDIR);
}

int Sysdeps<Rename>::operator()(const char *path, const char *new_path) {
	auto sc_ret = syscall(SYS_RENAME, path, new_path);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	auto sc_ret = syscall(SYS_LINK, old_path, new_path);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Linkat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags
) {
	auto sc_ret = syscall(SYS_LINKAT, olddirfd, old_path, newdirfd, new_path, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	auto sc_ret = syscall(SYS_SYMLINK, target_path, link_path);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	auto sc_ret = syscall(SYS_SYMLINKAT, target_path, dirfd, link_path);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
	auto sc_ret = syscall(SYS_CHMOD, pathname, mode);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old) {
	auto sc_ret = syscall(SYS_UMASK, mode);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (old)
		*old = static_cast<mode_t>(sc_ret);
	return 0;
}

int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
	auto sc_ret = syscall(SYS_KILL, pid, signal);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int
Sysdeps<Sigaction>::operator()(int signum, const struct sigaction *act, struct sigaction *oldact) {
	auto sc_ret = syscall(SYS_SIGACTION, signum, act, oldact);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *set, sigset_t *old) {
	if (how > 3)
		return EINVAL;
	(void)set;
	(void)old;
	return ENOSYS;
}

int Sysdeps<Utimensat>::operator()(
    int dirfd, const char *pathname, const struct timespec times[2], int flags
) {
	auto sc_ret = syscall(SYS_UTIMENSAT, dirfd, pathname, times, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Stat>::operator()(
    mlibc::fsfd_target target, int fd, const char *path, int flags, struct stat *statbuf
) {
	auto sc_ret = syscall(SYS_STAT, static_cast<int>(target), fd, path, flags, statbuf);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int
Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t max_size, ssize_t *length) {
	auto sc_ret = syscall(SYS_READLINK, path, buffer, max_size, length);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Readlinkat>::operator()(
    int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length
) {
	if (dirfd != AT_FDCWD)
		return ENOSYS;
	return sysdep<Readlink>(path, buffer, max_size, length);
}

int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
	unsigned long arg = va_arg(args, unsigned long);
	auto sc_ret = syscall(SYS_FCNTL, fd, request, arg);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (result)
		*result = static_cast<int>(sc_ret);
	return 0;
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	auto sc_ret = syscall(SYS_DUP, fd, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (newfd)
		*newfd = static_cast<int>(sc_ret);
	return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	auto sc_ret = syscall(SYS_DUP2, fd, newfd, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Pipe>::operator()(int *fds, int flags) {
	auto sc_ret = syscall(SYS_PIPE2, fds, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	auto sc_ret = syscall(SYS_POLL, fds, count, timeout, num_events);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	auto sc_ret = syscall(SYS_IOCTL, fd, request, arg);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (result)
		*result = static_cast<int>(sc_ret);
	return 0;
}

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TCGETS, attr);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcsetattr>::operator()(int fd, int optional_action, const struct termios *attr) {
	int req;
	switch (optional_action) {
		case TCSANOW:
			req = TCSETS;
			break;
		case TCSADRAIN:
			req = TCSETSW;
			break;
		case TCSAFLUSH:
			req = TCSETSF;
			break;
		default:
			return EINVAL;
	}
	auto sc_ret = syscall(SYS_IOCTL, fd, req, attr);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcsendbreak>::operator()(int fd, int) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TCSBRK, 0);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcflow>::operator()(int fd, int action) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TCXONC, action);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcflush>::operator()(int fd, int queue) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TCFLSH, queue);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcdrain>::operator()(int fd) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TCSBRK, 1);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize *winsz) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TIOCGWINSZ, winsz);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize *winsz) {
	auto sc_ret = syscall(SYS_IOCTL, fd, TIOCSWINSZ, winsz);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

pid_t Sysdeps<GetPid>::operator()() {
	auto sc_ret = syscall(SYS_GETPID);
	if (int e = sc_error(sc_ret); e)
		return (pid_t)-e;
	return static_cast<pid_t>(sc_ret);
}

uid_t Sysdeps<GetUid>::operator()() {
	auto sc_ret = syscall(SYS_GETUID);
	if (int e = sc_error(sc_ret); e)
		return (uid_t)-e;
	return static_cast<uid_t>(sc_ret);
}

uid_t Sysdeps<GetEuid>::operator()() {
	auto sc_ret = syscall(SYS_GETEUID);
	if (int e = sc_error(sc_ret); e)
		return (uid_t)-e;
	return static_cast<uid_t>(sc_ret);
}

gid_t Sysdeps<GetGid>::operator()() {
	auto sc_ret = syscall(SYS_GETGID);
	if (int e = sc_error(sc_ret); e)
		return (gid_t)-e;
	return static_cast<gid_t>(sc_ret);
}

gid_t Sysdeps<GetEgid>::operator()() {
	auto sc_ret = syscall(SYS_GETEGID);
	if (int e = sc_error(sc_ret); e)
		return (gid_t)-e;
	return static_cast<gid_t>(sc_ret);
}

pid_t Sysdeps<GetPpid>::operator()() {
	auto sc_ret = syscall(SYS_GETPPID);
	if (int e = sc_error(sc_ret); e)
		return (pid_t)-e;
	return static_cast<pid_t>(sc_ret);
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t* pgid) {
	auto sc_ret = syscall(SYS_GETPGID, pid);
	if (int e = sc_error(sc_ret); e)
		return (pid_t)-e;
	*pgid = static_cast<pid_t>(sc_ret);
	return 0;
}

pid_t Sysdeps<GetTid>::operator()() {
	auto sc_ret = syscall(SYS_GETTID);
	if (int e = sc_error(sc_ret); e)
		return (pid_t)-e;
	return static_cast<pid_t>(sc_ret);
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
	auto sc_ret = syscall(SYS_FACCESSAT, AT_FDCWD, path, mode, 0);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}
int Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags) {
	auto sc_ret = syscall(SYS_FACCESSAT, dirfd, pathname, mode, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	return 0;
}

} // namespace mlibc
