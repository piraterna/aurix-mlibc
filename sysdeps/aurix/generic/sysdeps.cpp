#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <abi-bits/vm-flags.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <stdint.h>
#include <string.h>

#define SYS_EXIT 0
#define SYS_OPEN 1
#define SYS_READ 2
#define SYS_WRITE 3
#define SYS_CLOSE 4
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

namespace {
inline int sc_error(long ret) { return ret < 0 ? -ret : 0; }
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
	(void)fd;
	return 0; // no tty stuff impl
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

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	auto sc_ret = syscall(SYS_GETCWD, buffer, size);
	if (int e = sc_error(sc_ret); e)
		return e;
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

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags,
					 struct rusage *, pid_t *ret_pid) {
	auto sc_ret = syscall(SYS_WAITPID, pid, status, flags);
	if (int e = sc_error(sc_ret); e)
		return e;
	if (ret_pid)
		*ret_pid = static_cast<pid_t>(sc_ret);
	return 0;
}


} // namespace mlibc
