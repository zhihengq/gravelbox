#include "tracer.h"
#include <utils.h>
#include <exceptions.h>

#include <cassert>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include <functional>

namespace GravelBox {

namespace TracerDetails {

using Utils::check;

void run_with_callbacks(const std::vector<std::string> &args, const std::function<bool(user_regs_struct)> &callback) {
	pid_t child = Utils::spawn(
		args, []() { ::ptrace(PTRACE_TRACEME, 0, nullptr, nullptr); });
	// wait for child process to be ready for trace
	int wstatus;
	check(::waitpid(child, &wstatus, 0));
	if (WIFEXITED(wstatus))
		// child process fails to start and has printed the error
		throw ChildExitException{WEXITSTATUS(wstatus)};

	// kill target if GravelBox is killed
	check(::ptrace(PTRACE_SETOPTIONS, child, nullptr, PTRACE_O_EXITKILL));
	while (true) {
		check(::ptrace(PTRACE_SYSCALL, child, nullptr, 0));
		// syscall entry
		check(::waitpid(child, &wstatus, 0));
		if (WIFEXITED(wstatus)) {
			break;
		} else {
			user_regs_struct regs;
			check(::ptrace(PTRACE_GETREGS, child, nullptr, &regs));
			callback(regs);
		}
		check(::ptrace(PTRACE_SYSCALL, child, nullptr, 0));
		// syscall entry
		check(::waitpid(child, &wstatus, 0));
		if (WIFEXITED(wstatus)) {
			break;
		}
	}
}

}  // namespace TracerDetails
}  // namespace GravelBox
