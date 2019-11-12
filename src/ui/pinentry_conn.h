#ifndef PINENTRY_CONN_H_
#define PINENTRY_CONN_H_

#include <exceptions.h>
#include <utils.h>

#include <sstream>

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

constexpr uint32_t kCancel = 83886179;

namespace GravelBox {

class PinentryConn {
  public:
	/**
	 * Default constructor.
	 */
	PinentryConn() noexcept : os_(odev_), is_(idev_) {
		os_.exceptions(std::ios::eofbit | std::ios::badbit);
		is_.exceptions(std::ios::eofbit | std::ios::badbit);
	}

	/**
	 * Copy constructor. (deleted)
	 */
	PinentryConn(const PinentryConn &) = delete;

	/**
	 * Copy assignment. (deleted)
	 */
	PinentryConn &operator=(const PinentryConn &) = delete;

	/**
	 * Establish connection with pinentry with the pipe file descriptors.
	 *
	 * @param pipe_in file descriptor of the pipe from pinetry.
	 * @param pipe_out file descriptor of the pipe to pinentry.
	 */
	void open(Utils::Fd pipe_in, Utils::Fd pipe_out);

	/**
	 * Close the pinentry connection.
	 */
	void close() {
		os_ << "BYE" << std::endl;
		ok();
	}

	/**
	 * Confirm with a message.
	 *
	 * @param message message to display.
	 * @return true if the user confirms.
	 * @return false if the user cancels the operation.
	 */
	bool confirm(const std::string &message) {
		os_ << "SETDESC " << message << std::endl;
		ok();
		os_ << "SETOK Allow" << std::endl;
		ok();
		os_ << "SETCANCEL Deny" << std::endl;
		ok();
		os_ << "CONFIRM" << std::endl;
		Response r = recv();
		if (r)
			return true;
		if (r.code == kCancel)
			return false;
		r.throw_error();
	}

	/**
	 * Result of `getpin` pinentry command.
	 */
	struct Password {
		bool cancelled;
		std::string password;
		operator bool() const noexcept { return !cancelled; }
	};

	/**
	 * Ask the user for a password.
	 *
	 * @param message descriptive message on the password dialog.
	 * @param prompt prompt on the password dialog.
	 * @param error error message on the password dialog, or empty for no error.
	 * @return Password user typed password, or cancelled.
	 */
	Password getpin(const std::string &message, const std::string &prompt,
					const std::string &error) {
		os_ << "SETDESC " << message << std::endl;
		ok();
		os_ << "SETPROMPT " << prompt << std::endl;
		ok();
		os_ << "SETOK OK" << std::endl;
		ok();
		os_ << "SETCANCEL Cancel" << std::endl;
		ok();
		if (!error.empty()) {
			os_ << "SETERROR " << error << std::endl;
			ok();
		}
		os_ << "GETPIN" << std::endl;
		Response r = recv();
		if (r)
			return {false, r.data};
		if (r.code == kCancel)
			return {true, ""};
		r.throw_error();
	}

  private:
	using ofd = boost::iostreams::file_descriptor_sink;
	using ifd = boost::iostreams::file_descriptor_source;

	ofd odev_;
	ifd idev_;
	boost::iostreams::stream<ofd> os_;
	boost::iostreams::stream<ifd> is_;

	/**
	 * Pinentry response.
	 */
	struct Response {
		uint32_t code;
		std::string data;
		operator bool() const noexcept { return code == 0; }
		[[noreturn]] void throw_error() const;
	};

	/**
	 * Receive a response from pinentry.
	 *
	 * @return Response the response data.
	 */
	Response recv();

	/**
	 * Assert that operation is successful.
	 * Throw `PinentryException` if there is any error.
	 */
	void ok() {
		Response r = recv();
		if (!r)
			r.throw_error();
	}
};

}  // namespace GravelBox

#endif  // PINENTRY_CONN_H_
