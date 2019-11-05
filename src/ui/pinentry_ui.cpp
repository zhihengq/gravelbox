#include "pinentry_ui.h"
#include <utils.h>

#include <unistd.h>
#include <sys/wait.h>

#include <string>

namespace GravelBox {

using Utils::check;

PinentryUI::PinentryUI(const std::string &pinentry) {
	// set up pipes
	int fds[4];
	check(::pipe(fds + 0));
	Utils::Fd to_pinentry_r(fds[0]);
	Utils::Fd to_pinentry_w(fds[1]);
	check(::pipe(fds + 2));
	Utils::Fd from_pinentry_r(fds[2]);
	Utils::Fd from_pinentry_w(fds[3]);
	const char *ttyname = ::ttyname(0);
	if (ttyname == nullptr)
		Utils::throw_system_error();
	pid_pinentry_ = Utils::spawn(
		{pinentry, "--ttyname", ttyname, "--lc-ctype", "utf-8"}, [&fds]() {
			check(::dup2(fds[0], 0));
			check(::dup2(fds[3], 1));
			for (int fd : fds)
				check(::close(fd));
		});

	// set up streams
	conn_.open(std::move(from_pinentry_r), std::move(to_pinentry_w));
}

PinentryUI::~PinentryUI() {
	conn_.close();
	int wstatus;
	check(::waitpid(pid_pinentry_, &wstatus, 0));
	assert(WIFEXITED(wstatus));
}

bool PinentryUI::ask(const std::string &syscall) {
	std::string message
		= "Do you allow the following system call?%0a" + syscall;
	return conn_.confirm(message);
}

}  // namespace GravelBox
