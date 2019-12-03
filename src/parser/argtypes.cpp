#include "argtypes.h"
#include <utils.h>

#include <sys/uio.h>

#include <cstdint>
#include <iomanip>
#include <locale>
#include <ostream>

namespace GravelBox {

constexpr size_t kMaxStrLen = 32;

void UnknownType::write(std::ostream &os, uint64_t value) const {
	os << "[0x" << std::hex << value << "]";
}

void SInt32Type::write(std::ostream &os, uint64_t value) const {
	os << std::dec << static_cast<int32_t>(value);
}

void UInt32Type::write(std::ostream &os, uint64_t value) const {
	os << std::dec << static_cast<uint32_t>(value);
}

void SInt64Type::write(std::ostream &os, uint64_t value) const {
	os << std::dec << static_cast<int64_t>(value);
}

void UInt64Type::write(std::ostream &os, uint64_t value) const {
	os << std::dec << static_cast<uint64_t>(value);
}

void PtrType::write(std::ostream &os, uint64_t value) const {
	if (value == 0)
		os << "NULL";
	else
		os << "0x" << std::hex << value;
}

void StrType::write(std::ostream &os, uint64_t value) const {
	char buf[kMaxStrLen];
	::iovec local = {buf, kMaxStrLen};
	::iovec remote[2] = {{reinterpret_cast<void *>(value), kMaxStrLen}};
	size_t num_remotes = 1;
	if (value / 4096 != (value + kMaxStrLen - 1) / 4096) {
		remote[0].iov_len = 4096 - value % 4096;
		remote[1].iov_base
			= reinterpret_cast<void *>((value / 4096 + 1) * 4096);
		remote[1].iov_len = kMaxStrLen - remote[0].iov_len;
		num_remotes = 2;
	}
	size_t bytes;
	try {
		bytes = Utils::check(
			::process_vm_readv(target_, &local, 1, remote, num_remotes, 0));
	} catch (const std::system_error &se) {
		if (se.code().value() == EFAULT) {
			os << "<fault>";
			return;
		} else {
			throw se;
		}
	}
	os << '\"';
	size_t i;
	for (i = 0; i < bytes; i++) {
		if (buf[i] == '\0')
			break;
		switch (buf[i]) {
		case '\\':
			os << "\\\\";
			break;
		case '\n':
			os << "\\n";
			break;
		case '\t':
			os << "\\t";
			break;
		default:
			if (std::isprint(buf[i])) {
				os << buf[i];
			} else {
				os << "\\x" << std::hex << std::setfill('0') << std::setw(2)
				   << static_cast<uint16_t>(buf[i]);
			}
		}
	}
	os << '\"';
	if (i == kMaxStrLen)
		os << "...";
}

}  // namespace GravelBox
