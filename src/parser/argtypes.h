#ifndef ARGTYPES_H_
#define ARGTYPES_H_

#include <cstdint>
#include <ostream>

namespace GravelBox {

class ArgType;

/**
 * An argument value with type info.
 * This class uses operator<< overload.
 */
class Arg {
	friend std::ostream &operator<<(std::ostream &os, const Arg &arg);
	friend class ArgType;

  private:
	const ArgType &type_;
	uint64_t value_;
	Arg(const ArgType &type, uint64_t arg) : type_(type), value_(arg) {}
};

/**
 * ABC for all argument types.
 */
class ArgType {
  public:
	/**
	 * Convert the argument register value to an `Arg` object.
	 *
	 * @param arg argument register value.
	 * @return Arg object.
	 */
	Arg operator()(uint64_t arg) const noexcept { return {*this, arg}; }

	/**
	 * Destroy the `ArgType` object
	 */
	virtual ~ArgType() {}

	/**
	 * Print an argument in this type.
	 *
	 * @param os the output stream.
	 * @param value argument register value.
	 */
	virtual void write(std::ostream &os, uint64_t value) const = 0;
};

/**
 * Operator overload for argument output.
 *
 * @param os an output stream
 * @param arg an `ArgWriter` object
 * @return `os`
 */
inline std::ostream &operator<<(std::ostream &os, const Arg &arg) {
	arg.type_.write(os, arg.value_);
	return os;
}

/**
 * Unknown argument type.
 */
struct UnknownType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
};

/**
 * Signed 32-bit integer type.
 */
struct SInt32Type : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
};

/**
 * Unsigned 32-bit integer type.
 */
struct UInt32Type : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
};

/**
 * Signed 64-bit integer type.
 */
struct SInt64Type : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
};

/**
 * Unsigned 64-bit integer type.
 */
struct UInt64Type : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
};

/**
 * Pointer type.
 * Printed in hexadecimal.
 */
struct PtrType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
};

}  // namespace GravelBox

#endif  // ARGTYPES_H_
