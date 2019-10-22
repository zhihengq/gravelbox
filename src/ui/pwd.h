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
	 * Deault username = "admin", pwd = "admin",
	 * locked = false (screen is unlocked).
	 */
	Pwd() = default; 

	/**
	 * Creates a new user-name-pwd combination.
	 * Deault username = admin, pwd = admin, screen is unlocked.
	 * 
	 * @param username  name of user
	 * @param pwd	    password to lock/unlock the screen
	 */
	Pwd(const std::string &username, const std::string &pwd)
		: username_(username), pwd_(pwd), locked_(false) {}

	/**
	 * Changes the current pwd if the given username and old password
	 * are correct. When locked, user cannot change password.
	 * 
	 * @param username  name of user
	 * @param old_pwd   original password
	 * @param new_pwd   new password
	 * @return true     if successfully changed pwd
	 */
	bool change_pwd(const std::string &username, const std::string &old_pwd, 
		const std::string &new_pwd);

	/**
	 * Lock the screen.
	 * 
	 * @return true   if successfully locked
	 */
	bool lock();

	/**
	 * Unlock the screen if the given password is correct.
	 * 
	 * @param pwd   password to unlock the screen
	 * @return true     if successfully unlocked
	 */
	bool unlock(const std::string &pwd);

	/**
	 * Return the locking status
	 * 
	 * @return true     if the screen is locked
	 */
	bool is_locked() {return locked_;};
  private:
	std::string username_ = "admin", pwd_ = "admin";
	bool locked_ = false;
};
	
}  // namespace GravelBox

#endif  // _PSW_H_
