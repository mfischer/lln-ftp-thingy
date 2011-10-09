#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h> // fork
#include <string.h> // strtok

#include "constants.h"
#include "utils.h"
#include "handlers.h"

#define NB_CMDSERVER 5
#define NB_CMDCLIENT 3
#define CMD_SIZE 10

/* List of commands on the client */
static char commandClient [NB_CMDCLIENT][CMD_SIZE] = { 
	"lpwd", 
	"lcd", 
	"lls" 
};

/* To check if it's a command which is executed on the client */
int isInCommandClient(char * command) {
	unsigned i = 0;
	for( ; i < NB_CMDCLIENT; i++) {
		if(!strcmp(command, commandClient[i])) return 0;
	}
	return -1;
}

int
init_data_connection (int connfd, unsigned int port)
{

	struct sockaddr_in myaddr;
	/* Set up the stuff for the listenfd */
	memset ((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family      = AF_INET;
	/* FIXME: Do a getsockname and have a random port */
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	myaddr.sin_port        = htons(9000);

	/* We try to bind to port 7000 on any address */
	int listenfd;
	listenfd = socket (AF_INET, SOCK_STREAM, 0); // Création d'un point de communication

	int info = 0;
	info = bind (listenfd, (const struct sockaddr*) &myaddr, sizeof (myaddr)); // 2me étape: chaque processus attache son socket à un port 
	if (info < 0)
		exit_error("Something went wrong with bind");
	/* We mark the socket as a passive, i.e. listening socket */
	info = listen (listenfd, BACKLOG);
	if (info < 0)
		exit_error("Something went wrong with listen");

	socklen_t clientlen = (socklen_t) sizeof (myaddr);
	/* Creation of 2 ports to receive data */
	unsigned int p1, p2;
	generate_client_ports(&p1, &p2, 9000);
	size_t size = MAXLINE;
	char buf[MAXLINE];
	snprintf (buf, size, "PORT %u,%u,%u,%u,%u,%u", 127, 0, 0, 1, p1, p2);
	printf ("[DEBUG] %s\n", buf);
	sock_print_nostat (connfd, buf);
	our_readline (buf, connfd);
	printf ("[DEBUG] %s\n", buf);
	/* FIXME: Here we do a our_readline to check for the status */

	/* Wainting answer of the server */
	listenfd = accept (listenfd, (struct sockaddr*) &myaddr, &clientlen);
	printf ("[DEBUG] We accepted\n");
	return listenfd;
}

/* Function which analyzes the command */
void analyseCommandLine(int connfd) {
	char* readbuf = calloc(MAXLINE + 1, sizeof(char));
	char* command;
	char separator = ' ';
	//int datafd;

	while(1) {
		printf("# ");
		readbuf = gets(readbuf); // Read command line in the shell 
		command = strtok(readbuf, &separator); // Take the first argument
		/* Local commands */
		if(isInCommandClient(command) != -1) {
			// Command found, we execute it
			// We remove the first character of the command
			char * line = calloc(256, sizeof(char));
			char * arg;
			size_t i = 1;
			while(i < strlen(command)) {
				line[i - 1] = command[i];
				i++;
			}
			// We copy the rest of the command line
			while(NULL != (arg = strtok(NULL, &separator))) {
				line = strcat(line, " ");
				line = strcat(line, arg);
			}
			system(line); // We execute the line
			free(line); 
		} 
		else if(!strcmp(command, "pwd")) {
			sock_print_nostat (connfd, "PWD");
		}
		else if(!strcmp(command, "cd")) {
			sock_print_nostat (connfd, "CWD chemin");
		} 
		else if(!strcmp(command, "ls"))
		{
			int listenfd = init_data_connection (connfd, 9000);
			char buf[MAXLINE];
			our_readline (buf, connfd);
			printf ("[DEBUG]: %s\n", buf);

			sock_print_nostat (connfd, "LIST");
			ssize_t len;
			for(;;)
			{
				len = read (listenfd, (void *) buf, MAXLINE * sizeof(char));
				/*printf ("[DEBUG] Read %d bytes\n",(int) len);*/
				if (len <= 0)
				{
					close (listenfd);
				}
			}

			close (listenfd);
			break;
		}
		else if(!strcmp(command, "bye")) {
			//quit_handler (datafd, connfd);
			break;
		} else if(!strcmp(command, "get")) {
			// get a file
			//retr_handler (datafd, cmdptr, connfd);
		} else if(!strcmp(command, "put")) {
			// put a file
			//				stor_handler (datafd, cmdptr, connfd);
		} else {
			printf("Invalid used of the command: %s" , command);
		}
		// Re-Initialization
		free(readbuf);
		readbuf = calloc(MAXLINE + 1, sizeof(char));
	}
	free(readbuf);
}

int main(int argc, char** argv) 
{
	int connfd;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];

	if (argc !=2)
		exit_error ("Bad Arguments! \nUsage : ./client <IPServerAdress>");

	/* Set up the stuff for the listenfd */
	memset ((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port        = htons(CMD_PORT);

	/* Open a TCP socket (an Internet stream socket) */
	if ( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_error ("Socket initialization error");

	/* We try to connect to port 7000 on the server */
	if (connect (connfd , (struct sockaddr *) &servaddr , sizeof ( servaddr )) < 0)
		exit_error ("connect error");

	/* This will be the server's welcome msg */
	our_readline (buf, connfd);
	printf ("%s\n", buf);

	analyseCommandLine(connfd);

	return 0;
}
