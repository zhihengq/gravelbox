#include "tracer.h"
#include <utils.h>
#include <exceptions.h>

#include <linux/version.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/user.h>

// custom ptrace declearation to use kernel 5.3 ptrace definition.
// switch back to glibc sys/ptrace.h when glibc is updated.
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 3, 0)
#error GravelBox requires Linux kernel version >= 5.3
#endif
#include <linux/ptrace.h>
#include <linux/audit.h>
extern "C" {
	extern int32_t ptrace(int __request, ...) noexcept;
}

#include <cassert>
#include <functional>
#include <unordered_map>

namespace GravelBox {

namespace TracerDetails {

using Utils::check;

enum class ThreadStatus { USERSPACE, KERNELSPACE_ALLOW, KERNELSPACE_DENY };

int run_with_callbacks(
	const std::vector<std::string> &args, const std::string &std_in,
	const std::string &std_out, bool append_stdout,
	const std::string &std_err, bool append_stderr,
	const std::function<void(pid_t)> &pid_callback,
	const std::function<bool(const Utils::SyscallArgs &)> &syscall_callback) {
	// spawn child
	pid_t child = Utils::spawn(args, [&]() {
		check(::ptrace(PTRACE_TRACEME, 0, nullptr, nullptr));
		if (::raise(SIGSTOP) != 0)
			Utils::throw_system_error();
		if (std_in != "-") {
			Utils::Fd fd(check(::open(std_in.c_str(), O_RDONLY)));
			check(::dup2(fd, 0));
		}
		if (std_out != "-") {
			Utils::Fd fdout(check(::open(
				std_out.c_str(),
				O_WRONLY | O_CREAT | (append_stdout ? O_APPEND : 0),
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)));
			check(::dup2(fdout, 1));
		}
		if (std_err != "-") {
			Utils::Fd fderr(check(::open(
				std_err.c_str(),
				O_WRONLY | O_CREAT | (append_stderr ? O_APPEND : 0),
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)));
			check(::dup2(fderr, 2));
		}
	});
	// wait for child process to be ready for trace
	int wstatus;
	check(::waitpid(child, &wstatus, 0));
	if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
		// child process fails to start and has printed the error
		throw ChildExitException{WIFEXITED(wstatus) ? WEXITSTATUS(wstatus)
													: 128 + WTERMSIG(wstatus)};
	}
	assert(WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGSTOP);

	// set-up trace
	uint64_t options = PTRACE_O_TRACESYSGOOD | PTRACE_O_TRACECLONE
					   | PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
	options |= PTRACE_O_EXITKILL;  // kill target if GravelBox is killed
#else
#warning Linux kernel version < 3.11, PTRACE_O_KILLEXIT is disabled
#endif
	check(::ptrace(PTRACE_SETOPTIONS, child, nullptr, options));
	check(::ptrace(PTRACE_SYSCALL, child, nullptr, 0));
	std::unordered_map<pid_t, ThreadStatus> thread_status{
		{child, ThreadStatus::USERSPACE}};

	// start tracing
	while (true) {
		child = check(::waitpid(-1, &wstatus, 0));
		if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
			thread_status.erase(child);
			if (thread_status.size() == 0)
				return WIFEXITED(wstatus)
						   ? WEXITSTATUS(wstatus)
						   : 128 + WTERMSIG(wstatus);  // consistent with bash
			continue;
		}
		try {
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
						ptrace_syscall_info info;
						check(::ptrace(PTRACE_GET_SYSCALL_INFO, child,
									   sizeof(info), &info));
						assert(info.op == PTRACE_SYSCALL_INFO_ENTRY);
						assert(info.arch == AUDIT_ARCH_I386
							   || info.arch == AUDIT_ARCH_X86_64);
						pid_callback(child);
						Utils::SyscallArgs args
							= {info.entry.nr,
							   {
								   info.entry.args[0],
								   info.entry.args[1],
								   info.entry.args[2],
								   info.entry.args[3],
								   info.entry.args[4],
								   info.entry.args[5],
							   },
							   info.arch == AUDIT_ARCH_I386};
						if (syscall_callback(args)) {
							thread_status[child]
								= ThreadStatus::KERNELSPACE_ALLOW;
						} else {
							user_regs_struct regs;
							check(::ptrace(PTRACE_GETREGS, child, nullptr,
										   &regs));
							regs.orig_rax = -1;
							check(::ptrace(PTRACE_SETREGS, child, nullptr,
										   &regs));
							thread_status[child]
								= ThreadStatus::KERNELSPACE_DENY;
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
		} catch (const std::system_error &se) {
			if (se.code().value() == ESRCH) {
				// tracee died during stop
				// clean-up only when we see the exit notification
				continue;
			} else {
				throw se;
			}
		}
	}
}

}  // namespace TracerDetails
}  // namespace GravelBox
