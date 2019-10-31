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
	iovec local = {buf, kMaxStrLen};
	iovec remote = {reinterpret_cast<void *>(value), kMaxStrLen};
	ssize_t bytes
		= Utils::check(::process_vm_readv(target_, &local, 1, &remote, 1, 0));
	assert(bytes == kMaxStrLen); Utils::use(bytes);
	os << '\"';
	size_t i;
	for (i = 0; i < kMaxStrLen; i++) {
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
