#include "tracer.h"
#include <utils.h>
#include <exceptions.h>

#include <linux/version.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include <cassert>
#include <functional>
#include <unordered_map>

namespace GravelBox {

namespace TracerDetails {

using Utils::check;

enum class ThreadStatus { USERSPACE, KERNELSPACE_ALLOW, KERNELSPACE_DENY };

int run_with_callbacks(
	const std::vector<std::string> &args,
	const std::function<void(pid_t)> &pid_callback,
	const std::function<bool(user_regs_struct)> &syscall_callback) {
	// spawn child
	pid_t child = Utils::spawn(args, []() {
		check(::ptrace(PTRACE_TRACEME, 0, nullptr, nullptr));
		if (::raise(SIGSTOP) != 0)
			Utils::throw_system_error();
	});
	// wait for child process to be ready for trace
	int wstatus;
	check(::waitpid(child, &wstatus, 0));
	if (WIFEXITED(wstatus))
		// child process fails to start and has printed the error
		throw ChildExitException{WEXITSTATUS(wstatus)};
	assert(WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGSTOP);

	// set-up trace
	uint64_t options = PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACECLONE
					   | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
	options |= PTRACE_O_EXITKILL;  // kill target if GravelBox is killed
#else
	// TODO(qzh): find a better solution
#warning Linux kernel version < 3.11, PTRACE_I_KILLEXIT is disabled
#endif
	check(::ptrace(PTRACE_SETOPTIONS, child, nullptr, options));
	check(::ptrace(PTRACE_SYSCALL, child, nullptr, 0));
	std::unordered_map<pid_t, ThreadStatus> thread_status{
		{child, ThreadStatus::USERSPACE}};

	// start tracing
	while (true) {
		child = check(::waitpid(-1, &wstatus, 0));
		if (WIFEXITED(wstatus)) {
			thread_status.erase(child);
			if (thread_status.size() == 0)
				return WEXITSTATUS(wstatus);
			continue;
		}
		if (WIFSTOPPED(wstatus)) {
			switch (WSTOPSIG(wstatus)) {
			case SIGSTOP:
				if (thread_status.find(child) == thread_status.end())
					thread_status[child] = ThreadStatus::USERSPACE;
				break;
			case SIGTRAP | 0x80:
				// syscall-enter-stop or syscall-exit-stop
				switch (thread_status.at(child)) {
				case ThreadStatus::USERSPACE: {
					// syscall-enter-stop
					user_regs_struct regs;
					check(::ptrace(PTRACE_GETREGS, child, nullptr, &regs));
					pid_callback(child);
					if (syscall_callback(regs)) {
						thread_status[child] = ThreadStatus::KERNELSPACE_ALLOW;
					} else {
						regs.orig_rax = -1;
						check(::ptrace(PTRACE_SETREGS, child, nullptr, &regs));
						thread_status[child] = ThreadStatus::KERNELSPACE_DENY;
					}
					break;
				}
				case ThreadStatus::KERNELSPACE_DENY: {
					// syscall-exit-stop
					user_regs_struct regs;
					check(::ptrace(PTRACE_GETREGS, child, nullptr, &regs));
					regs.rax = -EPERM;
					check(::ptrace(PTRACE_SETREGS, child, nullptr, &regs));
					[[fallthrough]];
				}
				case ThreadStatus::KERNELSPACE_ALLOW: {
					// syscall-exit-stop
					thread_status[child] = ThreadStatus::USERSPACE;
					break;
				}
				}
				break;
			}
			check(::ptrace(PTRACE_SYSCALL, child, nullptr, 0));
		}
	}
}

}  // namespace TracerDetails
}  // namespace GravelBox
