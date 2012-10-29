#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	char s;
	int i, fd;
	if (argc != 0) {
		perror("usage: test <no of iterations>");
		exit(0);
	}
	s = '1';
	for (i = 0; i < atoi(argv[1]); i++) {
		fd = open("/media/dmmount/file", O_WRONLY | O_CREAT, 0666);
		write(fd, &s, 1);
		close(fd);
		sleep(61);
	}
	return 0;
}
