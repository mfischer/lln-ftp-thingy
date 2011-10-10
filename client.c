#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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
int
is_client_command(char * command) {
	unsigned i = 0;
	for( ; i < NB_CMDCLIENT; i++) {
		if(!strcmp(command, commandClient[i])) return 0;
	}
	return -1;
}

unsigned int
response_to_status (const char* s)
{
	unsigned int ret;
	sscanf (s, "%u", &ret);
	return ret;
}

int
init_data_connection (int connfd, unsigned int port)
{

	/* Set up the stuff for the data connection */
	struct sockaddr_in myaddr;
	memset ((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family      = AF_INET;
	/* FIXME: Do a getsockname and have a random port */
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	myaddr.sin_port        = htons(port);

	/* We try to bind to port 9000 on any address */
	int listenfd;
	int newfd;
	listenfd = socket (AF_INET, SOCK_STREAM, 0);

	int info = 0;
	info = bind (listenfd, (const struct sockaddr*) &myaddr, sizeof (myaddr));
	if (info < 0)
		exit_error("Something went wrong with bind");
	/* We mark the socket as a passive, i.e. listening socket */
	info = listen (listenfd, BACKLOG);
	if (info < 0)
		exit_error("Something went wrong with listen");

	socklen_t clientlen = (socklen_t) sizeof (myaddr);
	unsigned int p1, p2;
	generate_client_ports(&p1, &p2, port);
	size_t size = MAXLINE;
	char buf[MAXLINE];
	snprintf (buf, size, "PORT %u,%u,%u,%u,%u,%u", 127, 0, 0, 1, p1, p2);
	/*printf ("[DEBUG] %s\n", buf);*/
	sock_print_nostat (connfd, buf);
	our_readline (buf, connfd);
	if (FTP_CMD_OK != response_to_status (buf))
	{
		printf ("Server did not like the PORT command!\n");
	}

	/* Waiting answer of the server */
	newfd = accept (listenfd, (struct sockaddr*) &myaddr, &clientlen);
	close (listenfd);
	return newfd;
}


	void
data_transfer (int listenfd, int connfd, int outfd, char* buf)
{
	ssize_t len;
	for(;;)
	{
		len = read (listenfd, (void *) buf, MAXLINE * sizeof(char));
		printf ("[DEBUG] Read/Sent %d bytes\n", (int) len);
		write (outfd, (void*) buf, len);
		if (len <= 0)
		{
			close (listenfd);
			break;
		}
	}

}

/* Function which analyzes the command */
void
analize_command_line(int connfd)
{
	char* readbuf = calloc(MAXLINE + 1, sizeof(char));
	char* command;
	char separator = ' ';

	while(1) {
		printf("> ");
		readbuf = gets(readbuf); // Read command line in the shell 
		command = strtok(readbuf, &separator); // Take the first argument
		/* Local commands */
		if(is_client_command(command) != -1)
		{
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
			char buf[MAXLINE];
			our_readline (buf, connfd);
			if (FTP_CMD_OK == response_to_status (buf))
			{
				printf ("%s\n", buf+4);
			}
		}
		else if(!strcmp(command, "cd")) {
			sock_print_nostat (connfd, "CWD");
		} 
		else if(!strcmp(command, "ls"))
		{
			int listenfd = init_data_connection (connfd, 50000);
			char buf[MAXLINE];

			sprintf (buf, "LIST %s", readbuf+3);
			sock_print_nostat (connfd, buf);

			/* FIXME: WTF? Check what the server thinks about this */
			our_readline (buf, connfd);
			if (response_to_status (buf) != FTP_FILE_STATUS_OK_OPEN_CONN)
				printf ("[DEBUG] Status: %u\n", response_to_status (buf));

			data_transfer (listenfd, connfd, 1, buf);
		}
		else if(!strcmp(command, "bye"))
		{
			/*break;*/
		}
		else if(!strcmp(command, "get"))
		{
			int listenfd = init_data_connection (connfd, 50000);
			char buf[MAXLINE];
			char args[MAXPATH];
			sprintf (buf, "RETR %s", readbuf+4);
			sprintf (args, "local-%s", readbuf+4);
			sock_print_nostat (connfd, buf);

			int f = open (args,
					O_CREAT | O_WRONLY,
					S_IRWXU | S_IRWXG | S_IROTH);

			if (f < 0)
				perror ("");
			printf ("[DEBUG] Opened local file %s\n", args);

			/* Check what the server thinks about this */
			our_readline (buf, connfd);
			if (response_to_status (buf) != FTP_FILE_STATUS_OK_OPEN_CONN)
				printf ("[DEBUG] Status: %u\n", response_to_status (buf));

			printf ("[DEBUG] Starting datatransfer\n");
			data_transfer (listenfd, connfd, f, buf);
			close (f);
			our_readline (buf, connfd);
			if (response_to_status (buf) != FTP_FINISHED_CLOSING)
			{
				printf ("[DEBUG] Status: %u\n", response_to_status (buf));
				close (listenfd);
			}
		}
		else if(!strcmp(command, "put")) {
			int listenfd = init_data_connection (connfd, 50000);
			char buf[MAXLINE];

			char args[MAXPATH];
			sprintf (buf, "STOR remote-%s", readbuf+4);
			sprintf (args, "%s", readbuf+4);
			sock_print_nostat (connfd, buf);

			int f = open (args, O_RDONLY);

			if (f < 0)
				perror ("Opening the local file went wrong.");

			/* Check what the server thinks about this */
			our_readline (buf, connfd);
			if (response_to_status (buf) != FTP_FILE_STATUS_OK_OPEN_CONN)
				printf ("[DEBUG] Status: %u\n", response_to_status (buf));

			data_transfer (f, connfd, listenfd, buf);
			/* Cleanup ... */
			close (f);
			close (listenfd);
			our_readline (buf, connfd);
			if (response_to_status (buf) != FTP_FINISHED_CLOSING)
				printf ("[DEBUG] Status: %u\n", response_to_status (buf));
		}
		else {
			printf("Invalid use of the command: %s" , command);
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
		exit_error ("Bad Arguments!\nUsage : ./client <IPServerAdress>");

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

	analize_command_line(connfd);

	return 0;
}
