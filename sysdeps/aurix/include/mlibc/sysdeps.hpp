#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct AurixSysdepsTags : LibcPanic,
                          LibcLog,
                          Isatty,
                          Write,
                          TcbSet,
                          AnonAllocate,
                          AnonFree,
                          Seek,
                          Exit,
                          Close,
                          FutexWake,
                          FutexWait,
                          Read,
                          Open,
                          OpenDir,
                          ReadEntries,
                          Stat,
                          VmMap,
                          VmUnmap,
                          VmProtect,
                          ClockGet,
                          GetUid,
                          GetEuid,
                          GetGid,
                          GetEgid,
                          GetPid,
                          GetPpid,
                          GetCwd,
                          Fork,
                          Chdir,
                          Waitpid,
                          Execve {};

template <typename Tag>
using Sysdeps = SysdepOf<AurixSysdepsTags, Tag>;

} // namespace mlibc
