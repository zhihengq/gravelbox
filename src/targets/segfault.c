#include <fcntl.h>
#include <stdio.h>

int main() {
	int fd = open(NULL, O_RDONLY);
	perror("Error");
	return 0;
}
