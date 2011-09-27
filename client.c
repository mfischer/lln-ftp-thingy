#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "constants.h"

int main(int argc, char** argv) 
{
	int connfd;
	struct sockaddr_in servaddr;

	if (argc !=2)
	{
		printf ("Not enough arguments!\n");
		exit (EXIT_FAILURE);
	}

	/* Set up the stuff for the listenfd */
	memset ((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port        = htons(CMD_PORT);

	/* Open a TCP socket (an Internet stream socket)*/
	if ( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror ("socket error initalization");
		exit (EXIT_FAILURE);
	}

	/* We try to connect to port 7000 on the server */
	if (connect (connfd , (struct sockaddr *) &servaddr , sizeof ( servaddr )) < 0)
	{
		perror ("connect error");
		exit (EXIT_FAILURE);
	}
	/*client code */


	return 0;
}
