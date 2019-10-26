#ifndef SYSCALLDEF_H_
#define SYSCALLDEF_H_

#include "argtypes.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <ostream>
#include <utility>
#include <vector>

namespace GravelBox {

/**
 * A `SyscallDef` contains a signature of an system call.
 * A `SyscallDef` is not aware of its corresponding syscall number, so it will
 * ignore `args.orig_rax` and assumes the arguments are for this syscall.
 */
class SyscallDef {
  public:
	/**
	 * Construct a SyscallDef object with function name.
	 *
	 * @tparam T
	 * @param name the syscall function name.
	 */
	template <typename T>
	SyscallDef(T &&name) : fname_(std::forward<T>(name)) {}

	/**
	 * Move construct a SyscallDef object.
	 *
	 * @param def SyscallDef to be moved.
	 */
	SyscallDef(SyscallDef &&def)
		: fname_(std::move(def.fname_)), argtypes_(std::move(def.argtypes_)) {}

	/**
	 * Add a parameter type to the syscall.
	 *
	 * @param type the next parameter type.
	 */
	void add_param(const ArgType &type) { argtypes_.emplace_back(type); }

	/**
	 * Write the human readable string of the syscall.
	 *
	 * @param os output stream.
	 * @param args syscall argument registers.
	 * @return `os`
	 */
	std::ostream &write(std::ostream &os,
						const std::array<uint64_t, 6> &args) const {
		assert(argtypes_.size() <= 6);
		os << fname_ << '(';
		for (size_t i = 0; i < argtypes_.size(); i++) {
			if (i > 0)
				os << ", ";
			os << argtypes_[i](args[i]);
		}
		return os << ')';
	}

  private:
	std::string fname_;
	std::vector<std::reference_wrapper<const ArgType>> argtypes_;
};

}  // namespace GravelBox

#endif  // SYSCALLDEF_H_
