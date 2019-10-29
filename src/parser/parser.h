#ifndef PARSER_H_
#define PARSER_H_

#include "argtypes.h"
#include "syscalldef.h"
#include <type_traits.h>

#include <sys/user.h>
#include <sys/types.h>

#include <functional>
#include <string>
#include <unordered_map>

namespace GravelBox {

/**
 * Parser class parses syscall numbers to function names and arguments to
 * corresponding parameter types.
 */
class Parser {
  public:
	/**
	 * Construct a Parser object from a syscall definition file.
	 *
	 * @param def the path of the definition file.
	 */
	explicit Parser(const std::string &def);

	/**
	 * Parse syscall registers to human readable strings.
	 *
	 * @param regs user registers at syscall entry.
	 * @return a string representation of the syscall with arguments.
	 */
	std::string operator()(const user_regs_struct &regs) const noexcept;

	/**
	 * Set the pid of the target.
	 * The pid is used to parse string arguments, which requires reading the
	 * target's memory.
	 *
	 * @param pid the pid of the target.
	 */
	void setpid(pid_t pid) { target_ = pid; }

  private:
	pid_t target_;
	UnknownType unknown_;
	SInt32Type sint32_;
	UInt32Type uint32_;
	SInt64Type sint64_;
	UInt64Type uint64_;
	StrType str_;
	PtrType ptr_;
	const std::unordered_map<std::string, std::reference_wrapper<const ArgType>>
		argtypes_{{"unknown", unknown_}, {"flags", uint64_},
				  {"int32_t", sint32_},  {"uint32_t", uint32_},
				  {"int64_t", sint64_},  {"uint64_t", uint64_},
				  {"char*", str_},       {"void*", ptr_}};
	std::unordered_map<uint64_t, SyscallDef> syscall_map_;
};

static_assert(IsParser<Parser>::value, "Parser does not fulfill Parser");

}  // namespace GravelBox

#endif  // PARSER_H_
