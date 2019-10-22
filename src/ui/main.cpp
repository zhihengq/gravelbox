/**
 * To test pwd.
 */

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>

#include "./pwd.h"

static void Usage(char* progname) {
	std::cerr << "Usage: " << progname << std::endl;
	std::cout << "To lock the console: enter \"LOCK\"" << std::endl;
	std::cout << "Cannot change password when locked. "
		<< "To change the password: enter \"CHANGE PWD\"" << std::endl;
	std::cout << "To shut down the console: enter <ctr-D>" << std::endl;
}

int main(int argc, char** argv) {
	GravelBox::Pwd user;
	// create a Pwd if user wants a pwd
	// otherwise start the loop with default user+psw specified in pwd.h
	std::cout << "Enter \"yes\" to create a user&password combination.Case-sensitive." << std::endl;
	std::cout << "Otherwise use default user&psw specified in pwd.h" << std::endl;
	std::string input;
	std::cin >> input;
	if (input.compare("yes") == 0) {
		std::string username, pwd;
		std::cout << "Enter a valid username, length >= 1" << std::endl;
		std::cin >> username;	
		std::cout << "Enter a valid passward, length >= 1" << std::endl;
		std::cin >> pwd;
		user = GravelBox::Pwd(username, pwd);
	}
	Usage(argv[0]);

	while(1) {
		if (!user.is_locked()) { // normal input when unlocked
			std::cout << "Enter query:" << std::endl;
    			std::cin >> input;	
			if (std::cin.eof()) {
				printf("shutting down...\n");
				break;
			} else if (input.compare("CHANGEPWD") == 0) {
				std::string username, old_pwd, new_pwd;
				std::cout << "*Enter your username." << std::endl;
				std::cin >> username;	
				std::cout << "*Enter your old password" << std::endl;
				std::cin >> old_pwd;	
				std::cout << "*Enter your new" << std::endl;
				std::cin >> new_pwd;
				if (!user.change_pwd(username, old_pwd, new_pwd)) {
					printf("wrong user name/password, please try again\n");
				}
			} else if (input.compare("LOCK") == 0) {
				// could return a false in future cases
				if(user.lock()) {
					std::cout << "***SUCCESSFULLY LOCKED***" << std::endl;
					std::cout << "Enter the correct password to unlock." << std::endl;
				}
			}

		} else { // once locked, cannot do anything else but try to unlock
			std::cin >> input;
			while (!user.unlock(input)) {
				std::cout << "Enter the correct password to unlock." << std::endl;
				std::cin >> input;
			}
		}
		
  	}
	return EXIT_SUCCESS;	
}
