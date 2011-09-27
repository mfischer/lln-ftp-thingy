#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "constants.h"
#include "utils.h"

int main(int argc, char** argv) 
{
	int connfd;
	struct sockaddr_in servaddr;

	if (argc !=2)
		exit_error ("Not enough arguments!");

	/* Set up the stuff for the listenfd */
	memset ((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port        = htons(CMD_PORT);

	/* Open a TCP socket (an Internet stream socket)*/
	if ( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_error ("Socket initialization error");

	/* We try to connect to port 7000 on the server */
	if (connect (connfd , (struct sockaddr *) &servaddr , sizeof ( servaddr )) < 0)
		exit_error ("connect error");
	/*client code */


	return 0;
}
