#include <iostream>
#include <string>

#include "ui.h"
using namespace std;
namespace GravelBox {
bool UI::display(const std::string &syscall) const {
	cout << syscall << endl;
	return true;
}

int UI::confirm() const {
	string input = "";
	// might add a -1 for "MODIFY", other wise just use boolean as return type
	while (input.compare("YES") != 0 && input.compare("NO") != 0)  {
		cout << "type \"YES\" to resume the call, \"NO\" to reject" << endl;
		getline(cin, input);
	}
	if (input.compare("YES") == 0) {
		return 1;
	} else {
		return 0;
	}
}
}  // namespace GravelBox
