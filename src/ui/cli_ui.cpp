#include "cli_ui.h"

#include <iostream>
#include <string>

namespace GravelBox {

bool CliUI::ask(const std::string &syscall) const {
	while (true) {
		std::cerr << syscall << " [Y/n] ";
		std::string input;
		std::getline(std::cin, input);
		if (input.empty() || input == "Y" || input == "y")
			return true;
		if (input == "N" || input == "n")
			return false;
		std::cerr << "Invalid input" << std::endl;
	}
}

}  // namespace GravelBox
