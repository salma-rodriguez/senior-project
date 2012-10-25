#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
	int fd;
	char s = '1';
	fd = open("/media/dmmount", O_RDWR | O_CREAT);
	// for (i = 0; i < count; i++) {
		// lseek(fd, 0, SEEK_SET);
		write(fd, &s, 1);
	// }
	close(fd);
	sleep(20);
	fd = open("/media/dmmount", O_RDWR);
	// for (i = 0; i < count; i++) {
		// lseek(fd, 0, SEEK_SET);
		write(fd, &s, 1);
	// }
	close(fd);
	// close(fg);
}
