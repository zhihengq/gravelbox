#ifndef TRACER_H_
#define TRACER_H_

#include <type_traits.h>
#include <utils.h>

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace GravelBox {

namespace TracerDetails {

/**
 * Non-template run.
 * Spawn and trace the child process and calls the callback when an syscall is
 * intercepted. Return after the child process exits.
 *
 * @param args the arguments used to spawn the child process.
 * @param callback a callback function when an syscall is intercepted.
 * @return child process exit code.
 */
int run_with_callbacks(
	const std::vector<std::string> &args, const std::string &std_in,
	const std::string &std_out, bool append_stdout,
	const std::string &std_err, bool append_stderr,
	const std::function<void(pid_t)> &pid_callback,
	const std::function<bool(const Utils::SyscallArgs &)> &syscall_callback);

}  // namespace TracerDetails

/**
 * Tracer class spawns and traces child processes.
 */
template <typename Parser, typename Config, typename UI, typename Logger>
class Tracer {
  public:
	/**
	 * Construct a Tracer with a parser, a config, a UI, and a logger.
	 * The tracer will take ownership of the parser, config, ui, and logger objects.
	 *
	 * @param parser the parser object.
	 * @param config the config object
	 * @param ui the ui object.
	 * @param logger the logger object
	 */
	Tracer(std::unique_ptr<Parser> parser, std::unique_ptr<Config> config,
		   std::unique_ptr<UI> ui, std::unique_ptr<Logger> logger) noexcept
		: parser_(std::move(parser)), config_(std::move(config)),
		  ui_(std::move(ui)), logger_(std::move(logger)) {}

	/**
	 * Spawn and trace a child process.
	 * Return after the child process exits.
	 *
	 * @param args the arguments used to spawn the child process.
	 * @param std_in the redirected path of stdin, or "-" if not redirected.
	 * @param std_out the redirected path of stdout, or "-" if not redirected.
	 * @param append_stdout whether the redirected stdout should be opened in APPEND mode.
	 * @param std_err the redirected path of stderr, or "-" if not redirected.
	 * @param append_stderr whether the redirected stderr should be opened in APPEND mode.
	 */
	int run(const std::vector<std::string> &args, const std::string &std_in,
			const std::string &std_out, bool append_stdout,
			const std::string &std_err, bool append_stderr) const {
		return TracerDetails::run_with_callbacks(
			args, std_in, std_out, append_stdout, std_err, append_stderr,
			[&parser = *parser_](pid_t child) { parser.setpid(child); },
			[&parser = *parser_, &config = *config_, &ui = *ui_,
			 &logger = *logger_](const Utils::SyscallArgs &args) -> bool {
				auto syscall_str = parser(args);
				logger.write(syscall_str);
				switch (config.get_action(syscall_str)) {
				case Config::Action::ALLOW:
					return true;
				case Config::Action::ASK: {
					if (!ui.ask(syscall_str))
						return false;
					if (!config.has_password())
						return true;
					constexpr auto message
						= "Enter the user decision password to continue.";
					constexpr auto prompt = "password: ";
					typename UI::Password password
						= ui.ask_password(message, prompt, "");
					if (!password)
						return false;
					while (!config.verify_password(password.password)) {
						password = ui.ask_password(message, prompt,
												   "Incorrect password");
						if (!password)
							return false;
					}
					return true;
				}
				case Config::Action::DENY:
					return false;
				}
				assert(false);
				return false;
			});
	}

  private:
	std::unique_ptr<Parser> parser_;
	std::unique_ptr<Config> config_;
	std::unique_ptr<UI> ui_;
	std::unique_ptr<Logger> logger_;

	static_assert(IsParser<Parser>::value, "Tracer must take in a Parser");
	static_assert(IsConfig<Config>::value, "Tracer must take in a Config");
	static_assert(IsUI<UI>::value, "Tracer must take in an UI");
};

}  // namespace GravelBox

#endif  // TRACER_H_
