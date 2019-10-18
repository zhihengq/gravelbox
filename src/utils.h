#ifndef UTILS_H_
#define UTILS_H_

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace GravelBox {


/**
 * Common util functions.
 */
namespace Utils {

/**
 * Throw an `system_error` with the current `errno`.
 */
[[noreturn]] inline void throw_system_error() {
	throw std::system_error(errno, std::system_category());
}

/**
 * Check C return values and translate errors to C++ exceptions.
 *
 * @param retval return value of C API.
 * @return `retval` if there is no error (`retval` \f$\ge 0\f$).
 * @throw system_error if there is an error (`retval` \f$< 0\f$).
 */
template <typename T>
inline T check(T retval) {
	if (retval < 0)
		throw_system_error();
	return retval;
}

/**
 * Spawn a child process.
 *
 * @param args The command line arguments. The first argument will be used as
 *             the executable.
 * @param child_init A function that the child process should execute before
 *                   `exec`.
 * @return the child process pid.
 * @throw system_error if `fork()` or `exec` fails.
 */
inline pid_t spawn(
		const std::vector<std::string> &args,
		std::function<void()> child_init = {}) {
	assert(args.size() >= 1);

	// parse args
	auto argarray = std::make_unique<const char * []>(args.size() + 1);
	for (size_t i = 0; i < args.size(); i++)
		argarray[i] = args[i].c_str();
	argarray[args.size()] = nullptr;

	// fork and exec
	pid_t pid = check(::fork());
	if (pid == 0) {
		if (child_init)
			child_init();
		::execvp(args[0].c_str(), const_cast<char * const *>(argarray.get()));
		throw_system_error();
	} else {
		return pid;
	}
}

}  // namespace Utils
}  // namespace GravelBox

#endif  // UTILS_H_
