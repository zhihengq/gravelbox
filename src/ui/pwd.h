#ifndef _PSW_H_
#define _PSW_H_

#include <iostream>
#include <string>

namespace GravelBox {
/**
 * Pwd class stores a username-password combination that can
 * lock/unlock the command line interface
 */
class Pwd {
  public:
 	/**
	 * Creates a password with input from stdin
	 * locked = false (screen is unlocked).
	 */
	Pwd(); 

	/**
	 * Lock the screen.
	 * 
	 * @return true   if successfully locked
	 */
	bool lock();

	/**
	 * Unlock the screen if the user inputs the correct password.
	 * 
	 * @return true     if successfully unlocked
	 */
	bool unlock();

  private:
	std::string pwd_ = "admin";
	bool locked_ = false;
};
	
}  // namespace GravelBox

#endif  // _PSW_H_
