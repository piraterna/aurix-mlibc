#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#define SYS_EXIT  0
#define SYS_OPEN  1
#define SYS_READ  2
#define SYS_WRITE  3
#define SYS_CLOSE  4

#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	sysdep<Write>(1, msg, strlen(msg));
}

int Sysdeps<Write>::operator()(int fd, void const *buf, size_t size, ssize_t *ret) {
	*ret = syscall(SYS_WRITE, fd, buf, size);
	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	STUB();
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	STUB();
}

int Sysdeps<AnonFree>::operator()(void *, unsigned long) {	
	STUB();
}

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	
	STUB();
}

void Sysdeps<Exit>::operator()(int status) {
	syscall(SYS_EXIT, status);
	__builtin_unreachable();
}

int Sysdeps<Close>::operator()(int) {
	STUB();
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	STUB();
}
int Sysdeps<FutexWait>::operator()(int *, int, timespec const *) {
	STUB();
}
int Sysdeps<Read>::operator()(int, void *, unsigned long, long *) {
	STUB();
}
int Sysdeps<Open>::operator()(const char *, int, unsigned int, int *) {
	STUB();
}
int Sysdeps<VmMap>::operator()(void *, size_t, int, int, int, off_t, void **) {
	STUB();
}
int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	STUB();
}
int Sysdeps<ClockGet>::operator()(int, time_t *, long *) {
	STUB();
}

} // namespace mlibc
