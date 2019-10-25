#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

// We use type traits to avoid C++20 concepts.
// We can switch to concepts once C++20 is out.

#include <sys/user.h>
#include <sys/types.h>

#include <string>
#include <type_traits>
#include <utility>

namespace GravelBox {

template <typename T, typename = void>
struct IsParser : std::false_type {};

template <typename Parser>
struct IsParser<
	Parser,
	std::void_t<
		std::enable_if_t<std::is_same<decltype(std::declval<Parser>()(
										  std::declval<user_regs_struct>())),
									  std::string>::value>,
		std::enable_if_t<std::is_same<decltype(std::declval<Parser>().setpid(
										  std::declval<pid_t>())),
									  void>::value>>> : std::true_type {};

template <typename T, typename = void>
struct IsUI : std::false_type {};

template <typename UI>
struct IsUI<UI,
			std::void_t<std::enable_if_t<std::is_same<
				decltype(std::declval<UI>().ask(std::declval<std::string>())),
				bool>::value>>> : std::true_type {};

}  // namespace GravelBox

#endif  // TYPE_TRAITS_H_
