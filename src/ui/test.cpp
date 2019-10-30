/**
 * To test pwd.
 */

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

#include "./pwd.h"
#include "./ui.h"

static void Usage(char* progname) {
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
	while(1) {
		std::cout << "Enter query:" << std::endl;
    		//std::cin >> input;	cin >> doesn't read the RETURN
		getline(std::cin, input);
		if (std::cin.eof()) {
			std::cout << "Shutting Down..." << std::endl;
			break;
		} else if (input.compare("LOCK") == 0) {
			// only return true for now
			if(user.lock()) {
				std::cout << "***SUCCESSFULLY LOCKED***" << std::endl;
				if (user.unlock()) { // only return true for now
					std::cout << "***SUCCESSFULLY UNLOCKED***" << std::endl;
				}
			}
		}

  	}
	return EXIT_SUCCESS;
}
