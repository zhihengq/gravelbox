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
	pid_pinentry_ = Utils::spawn({pinentry, "-T", ttyname}, [&fds]() {
		check(::dup2(fds[0], 0));
		check(::dup2(fds[3], 1));
		for (int fd : fds)
			check(::close(fd));
	});

	// set up streams
	odev_pinentry_.open(to_pinentry_w.release(), boost::iostreams::close_handle);
	idev_pinentry_.open(from_pinentry_r.release(), boost::iostreams::close_handle);
	check_resp("OK Pleased to meet you");

	// set up pinentry
	os_pinentry_ << "SETTITLE GravelBox" << std::endl;
	check_resp();
}

PinentryUI::~PinentryUI() {
	os_pinentry_ << "BYE" << std::endl;
	check_resp("OK closing connection");
	int wstatus;
	check(::waitpid(pid_pinentry_, &wstatus, 0));
	assert(WIFEXITED(wstatus));
}

bool PinentryUI::ask(const std::string &syscall) {
	os_pinentry_ << "SETDESC " << syscall << std::endl;
	check_resp();
	os_pinentry_ << "MESSAGE" << std::endl;
	check_resp();
	return true;
}

}  // namespace GravelBox
