#include <bits/syscall.h>

using sc_word_t = __sc_word_t;

extern "C" long __do_syscall_ret(unsigned long ret) {
	return ret;
}

sc_word_t __do_syscall0(long sc) {
	sc_word_t ret;
	asm volatile (
		"int $0x80"
		: "=a"(ret)
		: "a"(sc)
		: "rcx", "r11", "memory"
	);
	return ret;
}

sc_word_t __do_syscall1(long sc, sc_word_t arg1) {
	sc_word_t ret;
	asm volatile (
		"int $0x80"
		: "=a"(ret)
		: "a"(sc), "D"(arg1)
		: "rcx", "r11", "memory"
	);
	return ret;
}

sc_word_t __do_syscall2(long sc, sc_word_t arg1, sc_word_t arg2) {
	sc_word_t ret;
	asm volatile (
		"int $0x80"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "S"(arg2)
		: "rcx", "r11", "memory"
	);
	return ret;
}

sc_word_t __do_syscall3(long sc, sc_word_t arg1, sc_word_t arg2, sc_word_t arg3) {
	sc_word_t ret;
	asm volatile (
		"int $0x80"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3)
		: "rcx", "r11", "memory"
	);
	return ret;
}

sc_word_t __do_syscall4(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
		sc_word_t arg4) {
	sc_word_t ret;
	asm volatile (
		"mov %5, %%r10\n\t"
		"int $0x80"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4)
		: "r10", "rcx", "r11", "memory"
	);
	return ret;
}

sc_word_t __do_syscall5(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
		sc_word_t arg4, sc_word_t arg5) {
	sc_word_t ret;
	asm volatile (
		"mov %5, %%r10\n\t"
		"mov %6, %%r8\n\t"
		"int $0x80"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3), "r"(arg4), "r"(arg5)
		: "r8", "r10", "rcx", "r11", "memory"
	);
	return ret;
}

sc_word_t __do_syscall6(long sc,
		sc_word_t arg1, sc_word_t arg2, sc_word_t arg3,
		sc_word_t arg4, sc_word_t arg5, sc_word_t arg6) {
	sc_word_t ret;
	asm volatile (
		"mov %5, %%r10\n\t"
		"mov %6, %%r8\n\t"
		"mov %7, %%r9\n\t"
		"int $0x80"
		: "=a"(ret)
		: "a"(sc), "D"(arg1), "S"(arg2), "d"(arg3),
		  "r"(arg4), "r"(arg5), "r"(arg6)
		: "r8", "r9", "r10", "rcx", "r11", "memory"
	);
	return ret;
}