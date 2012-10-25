#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

void error(char *msg) {
	perror(msg);
	exit(0);
}

int main(int argc, char **argv) {
	int sockfd, portno, n;
	struct sockaddr_in serveraddr;
	struct hostent *server;
	char *hostname;
	char buf[BUFSIZE];
	
	if (argc != 3) {
		fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
	}

	hostname = argv[1];
	portno = atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");

	server = gethostbyname(hostname);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host as %s\n", hostname);
		exit(0);
	}

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		(char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(portno);

	if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
		error("ERROR connecting");

	printf("Please enter msg: ");
	bzero(buf, BUFSIZE);
	fgets(buf, BUFSIZE, stdin);

	if((n = write(sockfd, buf, strlen(buf))) < 0)
		error ("ERROR writing to socket");

	bzero(buf, BUFSIZE);

	if ((n = read(sockfd, buf, BUFSIZE)) < 0)
		error("ERROR receiving acknowledgement from server");

	printf("Echo from server: %s\n", buf);
	close(sockfd);
	return 0;
}
