#ifndef PINENTRY_UI_H_
#define PINENTRY_UI_H_

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

  private:
	pid_t pid_pinentry_;
	boost::iostreams::file_descriptor_sink odev_pinentry_;
	boost::iostreams::file_descriptor_source idev_pinentry_;
	boost::iostreams::stream<boost::iostreams::file_descriptor_sink>
		os_pinentry_ = odev_pinentry_;
	boost::iostreams::stream<boost::iostreams::file_descriptor_source>
		is_pinentry_ = idev_pinentry_;

	void check_resp(const std::string &expected = "OK") {
		std::string s;
		std::getline(is_pinentry_, s);
		if (s != expected)
			throw PinentryException(s);
	}
};

static_assert(IsUI<PinentryUI>::value,
			  "PinentryUI does not fulfill UI concept");

};  // namespace GravelBox

#endif
