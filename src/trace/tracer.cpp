#include "tracer.h"
#include <utils.h>

#include <cassert>

#include <sys/wait.h>
#include <sys/ptrace.h>

namespace GravelBox {

using Utils::check;

void Tracer::run(const std::vector<std::string> &args) const {
	pid_t child = Utils::spawn(
			args, []() {::ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);});
	int wstatus;
	check(::waitpid(child, &wstatus, 0));
	if (WIFEXITED(wstatus))
		exit(WEXITSTATUS(wstatus));
	check(::ptrace(PTRACE_SETOPTIONS, child, nullptr, PTRACE_O_EXITKILL));
	do {
		check(::ptrace(PTRACE_CONT, child, nullptr, 0));
		check(::waitpid(child, &wstatus, 0));
	} while (!WIFEXITED(wstatus));
}

}  // namespace GravelBox
