#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

int main(int argc, char **argv) {
	std::vector<std::thread> threads;
	std::mutex m;
	for (int i = 1; i < argc; i++) {
		threads.emplace_back([&m, f = argv[i]]() {
			std::ifstream is(f);
			if (!is.is_open()) {
				std::scoped_lock lock(m);
				std::cerr << "Error: Cannot open " << f << std::endl;
				return;
			}
			std::string s;
			std::getline(is, s);
			std::scoped_lock lock(m);
			std::cout << f << ": " << s << std::endl;
		});
	}
	for (auto &t : threads)
		t.join();
}
