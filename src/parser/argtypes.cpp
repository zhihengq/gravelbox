#include "argtypes.h"

#include <ostream>

namespace GravelBox {

void UnknownType::write(std::ostream &os, uint64_t value) const {
	os << "[0x" << std::hex << value << "]";
}

std::string UnknownType::id() const noexcept {
	return "unknown";
}

void SIntType::write(std::ostream &os, uint64_t value) const {
	os << std::dec << static_cast<int64_t>(value);
}

std::string SIntType::id() const noexcept {
	return "int";
}

void UIntType::write(std::ostream &os, uint64_t value) const {
	os << std::dec << value;
}

std::string UIntType::id() const noexcept {
	return "uint";
}

void PtrType::write(std::ostream &os, uint64_t value) const {
	if (value == 0)
		os << "NULL";
	else
		os << "0x" << std::hex << value;
}

std::string PtrType::id() const noexcept {
	return "void*";
}

}  // namespace GravelBox
