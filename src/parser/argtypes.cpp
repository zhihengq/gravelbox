#include "argtypes.h"

#include <ostream>

namespace GravelBox {

void UnknownType::write(std::ostream &os, uint64_t value) const {
	os << "[0x" << std::hex << value << "]";
}

void SIntType::write(std::ostream &os, uint64_t value) const {
	os << std::dec << static_cast<int64_t>(value);
}

void UIntType::write(std::ostream &os, uint64_t value) const {
	os << std::dec << value;
}

void PtrType::write(std::ostream &os, uint64_t value) const {
	if (value == 0)
		os << "NULL";
	else
		os << "0x" << std::hex << value;
}

}  // namespace GravelBox
