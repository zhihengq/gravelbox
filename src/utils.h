#ifndef UTILS_H_
#define UTILS_H_

#include <unistd.h>

#include <array>
#include <cassert>
#include <cerrno>
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
 * @tparam T return value type.
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
 * System call registers.
 */
struct SyscallArgs {
	uint64_t number;
	std::array<uint64_t, 6> args;
	bool int80;
};

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
inline pid_t spawn(const std::vector<std::string> &args,
				   std::function<void()> child_init = {}) {
	assert(args.size() >= 1);

	// parse args
	auto argarray = std::make_unique<const char *[]>(args.size() + 1);
	for (size_t i = 0; i < args.size(); i++)
		argarray[i] = args[i].c_str();
	argarray[args.size()] = nullptr;

	// fork and exec
	pid_t pid = check(::fork());
	if (pid == 0) {
		if (child_init)
			child_init();
		::execvp(args[0].c_str(), const_cast<char *const *>(argarray.get()));
		throw_system_error();
	} else {
		return pid;
	}
}

/**
 * Wrap elements into an `std::array<>`
 *
 * @tparam T array element type
 * @tparam N parameter types
 * @param args array elements
 * @return std::array<T,sizeof...(args)>
 */
template <typename T, typename... N>
auto make_array(N &&... args) -> std::array<T, sizeof...(args)> {
	return {std::forward<N>(args)...};
}

/**
 * A file descriptor wrapper with ownership.
 */
class Fd {
  public:
	/**
	 * Construct an Fd guard from a raw file descriptor.
	 * The Fd object will take ownership of the file descriptor.
	 *
	 * @param fd the raw file descriptor.
	 */
	explicit Fd(int fd) noexcept : fd_(fd) {}

	/**
	 * Move constructor.
	 *
	 * @param fd the Fd object to be moved.
	 */
	Fd(Fd &&fd) noexcept : fd_(fd.fd_) { fd.fd_ = -1; }

	/**
	 * Destroy the Fd object and close the file descriptor.
	 */
	~Fd() {
		if (fd_ >= 0)
			check(::close(fd_));
	}

	/**
	 * Move assignment.
	 *
	 * @param fd the Fd object to be moved.
	 * @return Fd& this
	 */
	Fd &operator=(Fd &&fd) noexcept {
		if (this != std::addressof(fd)) {
			fd_ = fd.fd_;
			fd.fd_ = -1;
		}
		return *this;
	}

	/**
	 * Convert into a raw file descriptor.
	 *
	 * @return int
	 */
	operator int() const noexcept { return fd_; }

	/**
	 * Release the ownership of the file descriptor.
	 *
	 * @return int the raw file descriptor.
	 */
	int release() noexcept {
		int fd = fd_;
		fd_ = -1;
		return fd;
	}

	/**
	 * Default constructor.
	 */
	Fd() = default;

	/**
	 * Copy constructor.
	 */
	Fd(const Fd &) = delete;

	/**
	 * Copy assignment.
	 */
	Fd &operator=(const Fd &) = delete;

  private:
	int fd_ = -1;
};

}  // namespace Utils
}  // namespace GravelBox

#endif  // UTILS_H_
