#include <exceptions.h>
#include <ui/pinentry_ui.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

constexpr size_t kHashSize = 512 / 8;

int main(int argc, char **argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <config> <signature>"
				  << std::endl;
		return EXIT_FAILURE;
	}
	std::string config_path = argv[1];
	std::string sig_path = argv[2];

	try {
		std::string config;
		{
			std::ifstream file(config_path, std::ios::binary);
			if (!file)
				throw GravelBox::ConfigException(
					config_path, "GravelBox configuration",
					"Cannot open file \"" + config_path + '\"');
			file.exceptions(std::ios::eofbit | std::ios::failbit);
			config.assign(std::istreambuf_iterator<char>(file),
						  std::istreambuf_iterator<char>());
		}

		std::string key;
		{
			GravelBox::PinentryUI ui("pinentry");
			constexpr auto message
				= "Enter the configuration file signing key.";
			constexpr auto prompt = "key: ";
			GravelBox::PinentryUI::Password response
				= ui.ask_password(message, prompt, "");
			if (!response)
				return EXIT_FAILURE;
			key = std::move(response.password);
		}

		std::array<uint8_t, kHashSize> md;
		{
			auto r = HMAC(EVP_sha512(), key.data(), key.size(),
						  reinterpret_cast<const uint8_t *>(config.data()),
						  config.size(), md.data(), nullptr);
			if (r == nullptr) {
				// Crypto error for unknown reasons
				ERR_print_errors_fp(stderr);
				return EXIT_FAILURE;
			}
		}

		{
			std::ofstream file(sig_path, std::ios::binary);
			if (!file)
				throw GravelBox::ConfigException(
					config_path, "GravelBox configuration signature",
					"Cannot open file \"" + sig_path + '\"');
			file.exceptions(std::ios::eofbit | std::ios::failbit);
			file.write(reinterpret_cast<const char *>(md.data()), md.size());
		}

		std::cerr << config_path << " signed successfully" << std::endl;
		return EXIT_SUCCESS;

	} catch (const GravelBox::ConfigException &ce) {
		std::cerr << "Config error: " << ce.what() << std::endl;
		return EXIT_FAILURE;
	} catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
