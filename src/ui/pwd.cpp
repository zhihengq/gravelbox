#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "./pwd.h"

using namespace std;

static int getch() {
	int ch;
	struct termios t_old, t_new;
	// get old mode
	tcgetattr(STDIN_FILENO, &t_old);
	t_new = t_old;
	// hide input on stdin
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

	ch = getchar();
	// restore the old mode
	tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
	return ch;
}

static string my_getpwd() {
	const char BACKSPACE=127;
	const char RETURN=10;

	string password;
	unsigned char ch=0;
	while((ch=getch())!=RETURN) {
		if(ch==BACKSPACE) {
			if(password.length()!=0) {
                 		cout << "\b \b";
                 		password.resize(password.length()-1);
              		}
         	} else {
             		password+=ch;
                 	cout << '*';
         	}
    	}
 	cout << endl;
  	return password;
}

namespace GravelBox {

Pwd::Pwd() {
	cout << "Set a password <= 100 characters:" << endl;
	pwd_ = my_getpwd();
	locked_ = false;
}

bool Pwd::lock() {
	// always return true for now.
	// Might have other cases in the future to return false.
	locked_ = true;
	return true;
}

bool Pwd::unlock() {
	// always return true for now.
	string cur_pwd;
	do {
		cout << "Enter your password to unlock:" << endl;
		cur_pwd = my_getpwd();
	} while (pwd_.compare(cur_pwd) != 0);
	locked_ = false;
	return true;
}
}
