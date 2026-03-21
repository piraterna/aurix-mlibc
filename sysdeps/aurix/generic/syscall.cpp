#include <errno.h>

using sc_word_t = long;

extern "C" sc_word_t __do_syscall_ret(sc_word_t ret) {
	if(ret < 0 && ret >= -4095) {
		errno = (int)-ret;
		return -1;
	}

	errno = 0;
	return ret;
}

static inline sc_word_t __do_syscall0(long sc) {
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc)
		: "rcx", "r11", "memory"
	);
	return ret;
}

static inline sc_word_t __do_syscall1(long sc, sc_word_t arg1) {
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc),
		  "D"(arg1)
		: "rcx", "r11", "memory"
	);
	return ret;
}

static inline sc_word_t __do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2) {
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc),
		  "D"(arg1),
		  "S"(arg2)
		: "rcx", "r11", "memory"
	);
	return ret;
}

static inline sc_word_t __do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2,
		sc_word_t arg3) {
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc),
		  "D"(arg1),
		  "S"(arg2),
		  "d"(arg3)
		: "rcx", "r11", "memory"
	);
	return ret;
}

static inline sc_word_t __do_syscall4(long sc, sc_word_t arg1, sc_word_t arg2,
		sc_word_t arg3, sc_word_t arg4) {
	register sc_word_t r10 asm("r10") = arg4;
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc),
		  "D"(arg1),
		  "S"(arg2),
		  "d"(arg3),
		  "r"(r10)
		: "rcx", "r11", "memory"
	);
	return ret;
}

static inline sc_word_t __do_syscall5(long sc, sc_word_t arg1, sc_word_t arg2,
		sc_word_t arg3, sc_word_t arg4, sc_word_t arg5) {
	register sc_word_t r10 asm("r10") = arg4;
	register sc_word_t r8  asm("r8")  = arg5;
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc),
		  "D"(arg1),
		  "S"(arg2),
		  "d"(arg3),
		  "r"(r10),
		  "r"(r8)
		: "rcx", "r11", "memory"
	);
	return ret;
}

static inline sc_word_t __do_syscall6(long sc, sc_word_t arg1, sc_word_t arg2,
		sc_word_t arg3, sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
	register sc_word_t r10 asm("r10") = arg4;
	register sc_word_t r8  asm("r8")  = arg5;
	register sc_word_t r9  asm("r9")  = arg6;
	sc_word_t ret;
	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(sc),
		  "D"(arg1),
		  "S"(arg2),
		  "d"(arg3),
		  "r"(r10),
		  "r"(r8),
		  "r"(r9)
		: "rcx", "r11", "memory"
	);
	return ret;
}