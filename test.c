#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
	int fd;
	char s = '8';
	fd = open("/home/salma/Documents/file", O_RDWR, S_IRUSR | S_IWUSR);
	lseek(
	write(fd, &s, 1);
	close(fd);
}
