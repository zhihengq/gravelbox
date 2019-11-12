#ifndef PINENTRY_UI_H_
#define PINENTRY_UI_H_

#include "pinentry_conn.h"
#include <exceptions.h>
#include <type_traits.h>
#include <utils.h>

#include <sys/types.h>

#include <string>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <iostream>

namespace GravelBox {

/**
 * An UI based on pinentry.
 */
class PinentryUI {
  public:
	/**
	 * Result returned by `PinentryConn`.
	 */
	using Password = PinentryConn::Password;

	/**
	 * Construct a PinentryUI object and launch pinentry process.
	 *
	 * @param pinentry the pinentry program name.
	 */
	explicit PinentryUI(const std::string &pinentry);

	/**
	 * Destroy the PinentryUI object and join the pinentry process.
	 */
	~PinentryUI();

	PinentryUI(const PinentryUI &) = delete;
	PinentryUI &operator=(const PinentryUI &) = delete;

	/**
	 * Display syscall string and ask user for permission via pinentry.
	 *
	 * @param syscall human-readable string representation of the syscall.
	 * @return true if the user accepts the syscall.
	 * @return false if the user denies the syscall.
	 */
	bool ask(const std::string &syscall);

	/**
	 * Ask the user for a password.
	 *
	 * @param message descriptive message on the password dialog.
	 * @param prompt prompt on the password dialog.
	 * @param error error message on the password dialog, or empty for no error.
	 * @return Password user typed password, or cancelled.
	 */
	Password ask_password(const std::string &message, const std::string &prompt,
						  const std::string &error) {
		return conn_.getpin(message, prompt, error);
	}

  private:
	const pid_t pid_self_;
	pid_t pid_pinentry_;
	PinentryConn conn_;
};

static_assert(IsUI<PinentryUI>::value,
			  "PinentryUI does not fulfill UI concept");

};  // namespace GravelBox

#endif
