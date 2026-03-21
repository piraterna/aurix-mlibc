#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct AurixSysdepsTags :
	LibcPanic,
	LibcLog,
	Exit,
	Open,
	Read,
	Write,
	Close
{};

template<typename Tag>
using Sysdeps = SysdepOf<AurixSysdepsTags, Tag>;

} // namespace mlibc
