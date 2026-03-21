#include "mlibc/internal-sysdeps.hpp"
#include "mlibc/posix-sysdeps.hpp"
#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <sys/syscall.h>
#include <sys/syscall_nums.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <mlibc/arch-defs.hpp>
#include <sys/ioctl.h>
#include <poll.h>

DEFINE_SYSCALL1(exit, SYS_EXIT, int)
DEFINE_SYSCALL3(open, SYS_OPEN, const char *, int, mode_t)
DEFINE_SYSCALL3(read, SYS_READ, int, void *, size_t)
DEFINE_SYSCALL1(close, SYS_CLOSE, int)
DEFINE_SYSCALL3(write, SYS_WRITE, int, const void *, size_t)

namespace mlibc {

    void sys_libc_log(const char *message) {
        // stdout is always fd 1 on aurix
        __syscall_write(1, (char*)message, strlen(message));
        __syscall_write(1, "\n", 1);
    }

    void sys_libc_panic() {
        sys_libc_log("\x1f[1;31mmlibc panic detected!\x1f[0m");
        sys_exit(1);
    }

    void sys_exit(int status) {
        __syscall_exit(status);
        sys_libc_log("\x1f[1;31mmlibc: exit syscall returned unexpectedly\x1f[0m");
        sys_libc_panic();
        __builtin_unreachable();
    }

    int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
        long error = __syscall_open(pathname, flags, mode);
        if (error < 0) {
            return -error;
        }
        *fd = error;
        return 0;
    }

    int sys_read(int fd, void *buf, size_t n, ssize_t *bytes_read) {
        long err = __syscall_read(fd, buf, n);
        if (err < 0) {
            return -err;
        }
        *bytes_read = err;
        return 0; 
    }

    int sys_write(int fd, const void *buf, size_t n, ssize_t *bytes_written) {
        ssize_t err = __syscall_write(fd, buf, n);
        if (err < 0) {
            return -err;
        }
        *bytes_written = err;
        return 0;
    }

    int sys_close(int fd) {
        long ret = __syscall_close(fd);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

} // namespace mlibc