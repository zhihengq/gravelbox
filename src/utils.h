#ifndef UTILS_H_
#define UTILS_H_

#include <cerrno>
#include <cstdlib>
#include <unistd.h>

#include <functional>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace GravelBox {
namespace Utils {

[[noreturn]] inline void throw_system_error() {
	throw std::system_error(errno, std::system_category());
}

template <typename T>
inline T check(T retval) {
	if (retval < 0)
		throw_system_error();
	return retval;
}

inline pid_t spawn(
		const std::vector<std::string> &args,
		std::function<void()> child_init = {}) {
	// parse args
	auto argarray = std::make_unique<const char * []>(args.size() + 1);
	for (size_t i = 0; i < args.size(); i++)
		argarray[i] = args[i].c_str();
	argarray[args.size()] = nullptr;

	pid_t pid = ::fork();
	if (pid < 0)
		throw_system_error();
	if (pid == 0) {
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
