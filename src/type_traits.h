#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

// We use type traits to avoid C++20 concepts.
// We can switch to concepts once C++20 is out.

#include <utils.h>

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
		std::enable_if_t<
			std::is_same<decltype(std::declval<Parser>()(
							 std::declval<const Utils::SyscallArgs>())),
						 std::string>::value>,
		std::enable_if_t<std::is_same<decltype(std::declval<Parser>().setpid(
										  std::declval<const pid_t>())),
									  void>::value>>> : std::true_type {};

template <typename T, typename = void>
struct IsUI : std::false_type {};

template <typename UI>
struct IsUI<
	UI,
	std::void_t<
		typename UI::Password,
		std::enable_if_t<
			std::is_same<decltype(static_cast<bool>(
							 std::declval<const typename UI::Password>())),
						 bool>::value>,
		std::enable_if_t<std::is_same<
			decltype(std::declval<typename UI::Password>().password),
			std::string>::value>,
		std::enable_if_t<std::is_same<decltype(std::declval<UI>().ask(
										  std::declval<const std::string>())),
									  bool>::value>,
		std::enable_if_t<std::is_same<decltype(std::declval<UI>().ask_password(
										  std::declval<const std::string>(),
										  std::declval<const std::string>(),
										  std::declval<const std::string>())),
									  typename UI::Password>::value>>>
	: std::true_type {};

template <typename T, typename = void>
struct IsConfig : std::false_type {};

template <typename Config>
struct IsConfig<
	Config,
	std::void_t<
		typename Config::Action,
		std::enable_if_t<
			std::is_same<decltype(std::declval<const Config>().syscalldef()),
						 std::string>::value>,
		std::enable_if_t<
			std::is_same<decltype(std::declval<const Config>().pinentry()),
						 std::string>::value>,
		std::enable_if_t<
			std::is_same<decltype(std::declval<const Config>().max_str_len()),
						 size_t>::value>,
		std::enable_if_t<
			std::is_same<decltype(std::declval<const Config>().get_action(
							 std::declval<const std::string>())),
						 typename Config::Action>::value>,
		std::enable_if_t<
			std::is_same<decltype(std::declval<const Config>().has_password()),
						 bool>::value>,
		std::enable_if_t<
			std::is_same<decltype(std::declval<const Config>().verify_password(
							 std::declval<const std::string>())),
						 bool>::value>>> : std::true_type {};

}  // namespace GravelBox

#endif  // TYPE_TRAITS_H_
