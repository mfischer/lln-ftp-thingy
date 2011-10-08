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
		//	pwd_handler (datafd, connfd);
		} else if(!strcmp(command, "cd")) {
		//	cwd_handler (datafd, cmdptr, connfd);
		} else if(!strcmp(command, "ls")) {
			//list_handler (datafd, connfd);
		} else if(!strcmp(command, "bye")) {
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
	
	/*client code */
	analyseCommandLine(connfd);
 		   
	return 0;
}
