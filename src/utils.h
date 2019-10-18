#ifndef UTILS_H_
#define UTILS_H_

#include <errno.h>

#include <system_error>

namespace GravelBox {
namespace Utils {

[[noreturn]] inline void throw_system_error() {
	throw std::system_error(errno, std::system_category());
}

}
}

#endif  // UTILS_H_
