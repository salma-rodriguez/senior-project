#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// #define IN "/proc/dm-cache/in"
#define SP "/proc/dm-cache/sp"
#define UP "/proc/dm-cache/up"
#define DN "/proc/dm-cache/dn"

int proc_read(char *name, char **buf) {
	int fd, ret;
	fd = open(name, 0400, S_IRUSR | S_IWUSR);
	ret = read(fd, *buf, 1);
	close(fd);
	return -ret;
}

int proc_writ(char *name, char *buf) {
	int fd, ret;
	fd = open(name, 0200, S_IRUSR | S_IWUSR);
	ret = write(fd, buf, 1);
	close(fd);
	return -ret;
}

int main(void) {
	int fd;
	char *buf;
	buf = malloc(sizeof(char));
	if (proc_writ(IN, "7") < 0)
		printf("error writing: %d\n", errno);
	proc_read(IN, &buf);
	printf("content of in: %s\n", buf);
	proc_read(SP, &buf);
	printf("content of sp: %s\n", buf);
	proc_read(UP, &buf);
	printf("content of up: %s\n", buf);
	proc_read(DN, &buf);
	printf("content of dn: %s\n", buf);
	exit(0);
}
