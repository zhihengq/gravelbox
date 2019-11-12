#include "pinentry_conn.h"
#include <exceptions.h>

#include <sstream>

namespace GravelBox {

[[noreturn]] void PinentryConn::Response::throw_error() const {
	assert(code != 0);
	std::ostringstream oss;
	oss << code << ' ' << data;
	throw PinentryException(oss.str());
}

void PinentryConn::open(Utils::Fd pipe_in, Utils::Fd pipe_out) {
	odev_.open(pipe_out.release(), boost::iostreams::close_handle);
	idev_.open(pipe_in.release(), boost::iostreams::close_handle);
	ok();
	os_ << "SETTITLE GravelBox" << std::endl;
	ok();
}

PinentryConn::Response PinentryConn::recv() {
	std::string data;
	while (true) {
		std::string line;
		std::getline(is_, line);
		if (line.substr(0, 3) == "OK" || line.substr(0, 3) == "OK ") {
			return {0, data};
		} else if (line.substr(0, 4) == "ERR ") {
			std::istringstream iss(line.substr(4));
			uint32_t code;
			iss >> code;
			iss.get();
			std::getline(iss, data);
			return {code, data};
		} else if (line.substr(0, 2) == "D ") {
			data = line.substr(2);
		} else if (line.substr(0, 2) == "S ") {
			// no-op
		} else {
			throw PinentryException("Unknown response: \"" + line + "\"");
		}
	}
}

}  // namespace GravelBox
