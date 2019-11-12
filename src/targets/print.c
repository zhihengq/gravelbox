#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
	bool failure = false;
	for (int i = 1; i < argc; i++) {
		int fd;
		if ((fd = open(argv[i], O_RDONLY)) < 0) {
			perror("a");//argv[i]);
			failure = true;
			continue;
		}
		struct stat fdstat;
		if (fstat(fd, &fdstat) < 0) {
			perror("b");//argv[i]);
			close(fd);
			failure = true;
			continue;
		}
		off_t size = fdstat.st_size;
		off_t offset = 0;
		while (offset < size) {
			if (sendfile(STDOUT_FILENO, fd, &offset, size - offset) < 0) {
				perror("c");//argv[i]);
				failure = true;
				break;
			}
		}
		close(fd);
	}
	return failure ? EXIT_FAILURE : EXIT_SUCCESS;
}
