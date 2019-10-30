#include "pwd.h"
#include "cli_ui.h"

#include <iostream>

static void Usage(const char* progname) {
	std::cerr << "Usage: " << progname << std::endl;
	std::cout << "1. You must set a password at first." << std::endl;
	std::cout << "2. To lock the console: enter \"LOCK\"" << std::endl;
	std::cout << "3. To shut down the console: enter <ctr-D>" << std::endl;
}

int main(int argc, char** argv) {
	Usage(argv[0]);
	// test Pwd
	GravelBox::Pwd user;
	std::string input;
	while (true) {
		std::cout << "Enter query:" << std::endl;
		std::getline(std::cin, input);
		if (std::cin.eof()) {
			std::cout << "Shutting Down..." << std::endl;
			break;
		} else if (input.compare("LOCK") == 0) {
			// only return true for now
			if (user.lock()) {
				std::cout << "***SUCCESSFULLY LOCKED***" << std::endl;
				if (user.unlock()) { // only return true for now
					std::cout << "***SUCCESSFULLY UNLOCKED***" << std::endl;
				}
			}
		}

  	}
	return EXIT_SUCCESS;
}
