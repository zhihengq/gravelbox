#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>

namespace GravelBox {

/**
 * Exception when the target process has exited.
 * GravelBox should clean up and exit with the same exit code as the child.
 */
class ChildExitException : std::exception {
  public:
	/**
	 * Exit code of the child.
	 */
	const int exit_code;

	/**
	 * Construct a ChildExitException object.
	 *
	 * @param exit_code the target child process exit code.
	 */
	explicit ChildExitException(int exit_code) noexcept
		: exit_code(exit_code) {}

	/**
	 * Return "Target has exited".
	 *
	 * @return "Target has exited".
	 */
	const char *what() const noexcept override { return "Target has exited"; }
};

}  // namespace GravelBox

#endif  // EXCEPTION_H_
