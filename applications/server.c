/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
   */

#define MAX 100
#define BUFSIZE 1024
#define MIN(a,b) a < b ? a:b
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

void handler(int); /* function prototype */

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

int openfile(const char *path) {
	int oflags = O_CREAT | O_RDWR | O_TRUNC;
	int pflags = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	return open(path, oflags, pflags);
}

int opendir(const char *path) {
        int oflags = O_CREAT | O_RDWR | O_DIRECTORY;
        int pflags = S_IRWXU | S_IRGRP | S_IROTH;
        return open(path, oflags, pflags);
}

char* concat(int numargs, char* str, ...) {
        char *s;
        char *cat;
        va_list argp;
        s = malloc(1024 * sizeof(char));
        va_start(argp, str);
        while(numargs--) {
                cat = va_arg(argp, char *);
                s = strcat(s,cat);
        }
        va_end(argp);
        return s;
}

int main(int argc, char *argv[])
{
	int bread;
	int sockfd, newsockfd, portno, pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0)
		error("ERROR on binding");
	listen(sockfd, MAX);
	clilen = sizeof(cli_addr);

	while (1) {
		newsockfd = accept(sockfd,
				(struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			error("ERROR on accept");
		pid = fork();
		if (pid < 0)
			error("ERROR on fork");
		if (pid == 0)  {
			// close(sockfd);
			handler(newsockfd);
			exit(0);
		}
		else close(newsockfd);
	} /* end of while */
	close(sockfd);
	return 0; /* we never get here */
}

/****************** HANDLER() *********************
  There is a separate instance of this function
  for each connection.  It handles all communication
  once a connnection has been established.
 **************************************************/
void handler (int sock)
{
        char *buf[BUFSIZE];
        int fd, n, newsock;
        bzero(buf, BUFSIZE);

        if ((n = read(sock, buf, BUFSIZE - 1)) < 0)
                error("ERROR reading from socket\n");

        fprintf(stdout, "message received: %s\n", buf);

        // send acknowledgement back to client
        if ((n = write(sock, "got it!", 7)) < 0)
                error("ERROR writing to socket\n");

}
