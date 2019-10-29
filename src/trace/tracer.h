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
void run_with_callbacks(const std::vector<std::string> &args,
						const std::function<bool(user_regs_struct)> &callback);

}  // namespace TracerDetails

/**
 * Tracer class spawns and traces child processes.
 */
template <typename Parser, typename UI, typename Logger>
class Tracer {
  public:
	/**
	 * Construct a Tracer with a parser, a UI, and a logger.
	 * The tracer will take ownership of the parser, ui, and logger objects.
	 *
	 * @param parser the parser object.
	 * @param ui the ui object.
	 * @param logger the logger object
	 */
	Tracer(std::unique_ptr<Parser> parser, std::unique_ptr<UI> ui,
		   std::unique_ptr<Logger> logger) noexcept
		: parser_(std::move(parser)), ui_(std::move(ui)),
		  logger_(std::move(logger)) {}

	/**
	 * Spawn and trace a child process.
	 * Return after the child process exits.
	 *
	 * @param args the arguments used to spawn the child process.
	 */
	void run(const std::vector<std::string> &args) const {
		TracerDetails::run_with_callbacks(
			args,
			[&parser = *parser_, &ui = *ui_,
			 &logger = *logger_](const auto &regs) -> bool {
				auto syscall_str = parser(regs);
				logger.write(syscall_str);
				return ui.ask(syscall_str);
			});
	}

  private:
	std::unique_ptr<Parser> parser_;
	std::unique_ptr<UI> ui_;
	std::unique_ptr<Logger> logger_;

	static_assert(IsParser<Parser>::value, "Tracer must take in a Parser");
	static_assert(IsUI<UI>::value, "Tracer must take in an UI");
};

}  // namespace GravelBox

#endif  // TRACER_H_
