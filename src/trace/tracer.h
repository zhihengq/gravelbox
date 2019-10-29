#ifndef TRACER_H_
#define TRACER_H_

#include <type_traits.h>

#include <sys/user.h>

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
 */
void run_with_callbacks(
	const std::vector<std::string> &args,
	const std::function<void(pid_t)> &pid_callback,
	const std::function<bool(user_regs_struct)> &syscall_callback);

}  // namespace TracerDetails

/**
 * Tracer class spawns and traces child processes.
 */
template <typename Parser, typename UI>
class Tracer {
  public:
	/**
	 * Construct a Tracer with a parser and a UI.
	 * The tracer will take ownership of the parser and ui objects.
	 *
	 * @param parser the parser object.
	 * @param ui the ui object.
	 */
	Tracer(std::unique_ptr<Parser> parser, std::unique_ptr<UI> ui) noexcept
		: parser_(std::move(parser)), ui_(std::move(ui)) {}

	/**
	 * Spawn and trace a child process.
	 * Return after the child process exits.
	 *
	 * @param args the arguments used to spawn the child process.
	 */
	void run(const std::vector<std::string> &args) const {
		TracerDetails::run_with_callbacks(
			args, [&parser = *parser_](pid_t child) { parser.setpid(child); },
			[&parser = *parser_, &ui = *ui_](const auto &regs) -> bool {
				auto syscall_str = parser(regs);
				return ui.ask(syscall_str);
			});
	}

  private:
	std::unique_ptr<Parser> parser_;
	std::unique_ptr<UI> ui_;

	static_assert(IsParser<Parser>::value, "Tracer must take in a Parser");
	static_assert(IsUI<UI>::value, "Tracer must take in an UI");
};

}  // namespace GravelBox

#endif  // TRACER_H_
