#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 1024

void error(char *msg) {
	perror(msg);
	exit(0);
}

int send_req(char *hostname, int portno) {
	int sockfd, n;
	char *command, buf[BUFSIZE];
	struct sockaddr_in serveraddr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host as %s\n", hostname);
		exit(0);
	}

	command = "shell command goes here\n";

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		error("ERROR connecting");

	if((n = write(sockfd, command, strlen(command))) < 0)
		error ("ERROR writing to socket");

	bzero(buf, BUFSIZE);

	if ((n = read(sockfd, buf, BUFSIZE)) < 0)
		error("ERROR receiving acknowledgement from server");

	printf("Echo from server: %s\n", buf);
	close(sockfd);
	return 0;
}

int main(int argc, char **argv) {
	int fd, fr, portno;
	char *hostname, buf[1];
	if (argc != 3) 
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
	hostname = argv[1];
	portno = atoi(argv[2]);
	while(1) {
		/* may need to make this atomic */
		fr = open("/home/salma/Documents/input", O_RDWR);
		fd = open("/home/salma/Documents/output", O_RDWR);
		read(fd, buf, 1);
		if (!(buf[0]^'1')) {
			printf("sending request...\n");
			send_req(hostname, portno);
			sleep(2);
			write(fr, "1", 1);
			write(fd, "0", 1);
		}
		close(fd);
		close(fr);
		/* end of atomic operation */
	}
}
