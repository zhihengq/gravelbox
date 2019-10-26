#include "argtypes.h"

#include <cstdint>
#include <ostream>

namespace GravelBox {

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

}  // namespace GravelBox
