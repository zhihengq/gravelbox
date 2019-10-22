#include "./pwd.h"

namespace GravelBox {
bool Pwd::change_pwd(const std::string &username, const std::string &old_pwd, 
		const std::string &new_pwd) {
	if (username.compare(username_) == 0 && old_pwd.compare(pwd_) == 0) {
		pwd_ = new_pwd;
		return true;
	}
	return false;
}

bool Pwd::lock() {
	// always return true for now.
	// Might have other cases in the future to return false.
	locked_ = true;
	return true;
}

bool Pwd::unlock(const std::string &pwd) {
	if (pwd_.compare(pwd) == 0) {
		locked_ = false;
		return true;       
	}
	return false;
}
}
