#include "tracer.h"
#include <utils.h>

#include <unistd.h>
#include <sys/ptrace.h>
#include <memory>
#include <system_error>

namespace GravelBox {

void Tracer::run(const std::vector<std::string> &args) const {
	pid_t pid = ::fork();
	if (pid < 0)
		Utils::throw_system_error();
	if (pid == 0) {
		if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0)
			Utils::throw_system_error();
		auto argarray = std::make_unique<const char * []>(args.size() + 1);
		for (size_t i = 0; i < args.size(); i++)
			argarray[i] = args[i].c_str();
		argarray[args.size()] = nullptr;
		::execvp(args[0].c_str(), const_cast<char * const *>(argarray.get()));
		Utils::throw_system_error();
	} else {
	}
}

}  // namespace GravelBox
