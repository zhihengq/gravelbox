#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <sstream>

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

/**
 * Exception when failed to parse a config file.
 */
class ConfigException : std::exception {
  public:
	/**
	 * Construct a `ConfigException` object.
	 *
	 * @param path the file path of the config file.
	 * @param type the expected type description of the config file.
	 * @param details optional detailed error message.
	 */
	ConfigException(const std::string &path, const std::string &type,
					const std::string &details) {
		std::ostringstream oss;
		oss << "Config file \"" << path << "\" is not a valid " << type
			<< " file";
		if (!details.empty())
			oss << std::endl << details;
		what_ = oss.str();
	}

	/**
	 * Return the error message.
	 *
	 * @return "Config file {path} is not a valid {type} file".
	 */
	const char *what() const noexcept override { return what_.c_str(); }

  private:
	std::string what_;
};

/**
 * Exception from pinentry errors.
 */
class PinentryException : std::exception {
  public:
	/**
	 * Construct a PinentryException with error message from pinenetry.
	 *
	 * @param msg error message from pinentry.
	 */
	PinentryException(const std::string &msg) : what_(msg) {}

	/**
	 * Return the error message.
	 *
	 * @return error message.
	 */
	const char *what() const noexcept override { return what_.c_str(); }

  private:
	std::string what_;
};

}  // namespace GravelBox

#endif  // EXCEPTION_H_
