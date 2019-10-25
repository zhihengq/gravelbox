#ifndef ARGTYPES_H_
#define ARGTYPES_H_

#include <sys/types.h>

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
	const ArgType *type_;
	uint64_t value_;
	Arg(const ArgType *type, uint64_t arg) : type_(type), value_(arg) {}
};

/**
 * ABC for all argument types.
 */
class ArgType {
  public:
	/**
	 * Instantiate an `Arg` with an argument register value.
	 *
	 * @param arg the argument register value.
	 * @return Arg the argument to be printed.
	 */
	Arg operator()(uint64_t arg) const noexcept { return {this, arg}; }

	/**
	 * Cleanup the `ArgType` object.
	 */
	virtual ~ArgType() {}

	/**
	 * Print a value of this argument type.
	 *
	 * @param os the output stream.
	 * @param value an argument register value of this type.
	 */
	virtual void write(std::ostream &os, uint64_t value) const = 0;

	/**
	 * Return the string representation of the type.
	 * Used in the config file.
	 *
	 * @return the string id.
	 */
	virtual std::string id() const noexcept = 0;
};

/**
 * Operator overload for argument output.
 *
 * @param os an output stream
 * @param arg an `ArgWriter` object
 * @return `os`
 */
inline std::ostream &operator<<(std::ostream &os, const Arg &arg) {
	arg.type_->write(os, arg.value_);
	return os;
}

/**
 * Unknown argument type.
 */
struct UnknownType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
	std::string id() const noexcept override;
};

/**
 * Signed integer type.
 */
struct SIntType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
	std::string id() const noexcept override;
};

/**
 * Unsigned integer type.
 */
struct UIntType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
	std::string id() const noexcept override;
};

/**
 * Pointer type.
 * Printed in hexadecimal.
 */
struct PtrType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
	std::string id() const noexcept override;
};

/**
 * String type.
 * Read target memory and print the null-terminated string.
 */
struct StrType : public ArgType {
	void write(std::ostream &os, uint64_t value) const override;
	std::string id() const noexcept override;

	/**
	 * Set the target pid.
	 * This allows the type to read target's memory.
	 *
	 * @param pid the pid of the target.
	 */
	void setpid(pid_t pid);

	/**
	 * Cleanup the file descriptor used to read the target's memory.
	 */
	~StrType() override;
};

}  // namespace GravelBox

#endif  // ARGTYPES_H_
