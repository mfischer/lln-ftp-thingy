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

#define NB_CMDSERVER 5
#define NB_CMDCLIENT 3
#define CMD_SIZE 10

/* List of commands on the client */
static char commandClient [NB_CMDCLIENT][CMD_SIZE] = { 
	"lpwd", 
	"lcd", 
	"lls" 
};

/* List of simple commands on the server */
static char commandOnServer[NB_CMDSERVER][CMD_SIZE] = {
	"pwd",
	"cd",
	"ls"
};

/* To check if it's a command which is executed on the client */
int isInCommandClient(char * command) {
	unsigned i = 0;
	for( ; i < NB_CMDCLIENT; i++) {
		if(!strcmp(command, commandClient[i])) return 0;
	}
	return -1;
}

/* To check if it's a simple command which is executed on the server */
int isItCommandOnServer(char * command) {
	unsigned i = 0;
	for( ; i < NB_CMDSERVER; i++) {
		if(!strcmp(command, commandOnServer[i])) return 0;
	}
	return -1;
}

/* Function which analyzes the command */
void analyseCommandLine(char* readbuf) {
	printf("\n# ");		
	scanf("%s", readbuf); // Read command line in the shell 
	char* command;	
	char separator = ' ';
	command = strtok(readbuf, &separator); // Take the first argument
	while(1) {		
		if(isInCommandClient(command) != -1) {
			// Command found, we execute it
			puts("Sa mere");
			char * path = malloc(sizeof(char)*256);			
			char * arg;			
			size_t i = 1;
			while(i < strlen(command)) {
				path[i - 1] = command[i];
				i++;
			}
			
			while(NULL != (arg = strtok(NULL, &separator))) {
				path = strcat(path, " ");					
				path = strcat(path, arg);							
			}		
			system(path);
			free(path);
	
		} else if(isItCommandOnServer(command) != -1) {
			
		} else if(!strcmp(command, "get")) {
			// get a file
		} else if(!strcmp(command, "put")) {
			// put a file
		} else if(!strcmp(command, "bye")) {
			// Close the connection			
			//close();		
		} else {
			printf("Invalid used of the command: %s" , command);
		}
		printf("\n# ");				
		scanf("%s", readbuf); // Read command line in the shell 
	}
	printf("\nCommande Entrée : %s", command);
}

int main(int argc, char** argv) 
{
	/*int connfd;
	struct sockaddr_in servaddr;

	if (argc !=2)
		exit_error ("Not enough arguments!");

	 Set up the stuff for the listenfd 
	memset ((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port        = htons(CMD_PORT);

	Open a TCP socket (an Internet stream socket)
	if ( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_error ("Socket initialization error");*/

	/* We try to connect to port 7000 on the server 
	if (connect (connfd , (struct sockaddr *) &servaddr , sizeof ( servaddr )) < 0)
		exit_error ("connect error");*/
	/*client code */
	
	/* Creation of new process to read answer	*/
		
	
	
	char readbuf[MAXLINE + 1];//, writebuf[MAXLINE + 1];   
	/* Father Process uses to send data */
		 analyseCommandLine(readbuf);
    
	

	return 0;
}
