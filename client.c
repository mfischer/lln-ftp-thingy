#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include <histedit.h>

#include "constants.h"
#include "utils.h"
#include "handlers.h"

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
		printf ("Server did not like the PORT command!\n");

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
		/*printf ("[DEBUG] Read/Sent %d bytes\n", (int) len);*/
		write (outfd, (void*) buf, len);
		if (len <= 0)
		{
			close (listenfd);
			break;
		}
	}
}

char *
prompt(EditLine *e) {
  return "ftp> ";
}

int main(int argc, char** argv) 
{
	int connfd;
	struct sockaddr_in servaddr;
	char buf[MAXLINE];

	/* This holds all the state for our line editor */
	EditLine *el;
	History *myhistory;

	/* Temp variables */
	int count;
	const char *line;
	HistEvent ev;

	/* Initialize the EditLine state to use our prompt function and
		 vim style editing. */
	el = el_init(argv[0], stdin, stdout, stderr);
	el_set(el, EL_PROMPT, &prompt);
	el_set(el, EL_EDITOR, "vim");

	/* Initialize the history */
	myhistory = history_init();
	if (myhistory == 0) {
		fprintf(stderr, "history could not be initialized\n");
		return EXIT_FAILURE;
	}
	history(myhistory, &ev, H_SETSIZE, 800);
	el_set(el, EL_HIST, history, myhistory);

	char readbuf[MAXLINE];
	unsigned int tmp_port;
	if (argc !=2)
	{
		count = MAXLINE;
		printf ("Please type an address of type 127.0.0.1:7000\n");
		line = el_gets(el, &count);
		if (count > 0)
			history(myhistory, &ev, H_ENTER, line);
	}
	else
	{
		line = (char *) argv[1];
		count = strlen (line);
	}

	unsigned int h1, h2, h3, h4;
	sscanf (line, "%u.%u.%u.%u:%u", &h1, &h2, &h3, &h4, &tmp_port);
	snprintf (readbuf, count, "%u.%u.%u.%u", h1,h2,h3,h4);
	
	/* Set up the stuff for the listenfd */
	memset ((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(readbuf);
	servaddr.sin_port        = htons(tmp_port);

	/* Open a TCP socket (an Internet stream socket) */
	if ( (connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_error ("Socket initialization error");

	/* We try to connect to port 'tmp_port' on the server */
	if (connect (connfd , (struct sockaddr *) &servaddr , sizeof ( servaddr )) < 0)
		exit_error ("connect error");

	/* This will be the server's welcome msg */
	our_readline (buf, connfd);
	printf ("%s\n", buf+4);

	while (1)
	{
		char cmd[MAXLINE];
		char args[MAXLINE];
		line = el_gets(el, &count);
		/* This is kind of hacky ... but we have to get rid of the newlines */
		char* nl = strrchr (line, '\n');
		if (nl)
			*nl = '\0';

		if (count > 0)
		{
			history(myhistory, &ev, H_ENTER, line);
			sscanf (line, "%s %s", cmd, args);
			if(!strncmp (cmd, "logout", 6))
			{
				sock_print_nostat (connfd, "QUIT");
				our_readline (buf, connfd);
				if (response_to_status (buf) == FTP_BYE)
				{
					printf ("Closing connection to server...\n");
					close (connfd);
					return EXIT_SUCCESS;
				}
			}
			else if (!strncmp (cmd, "ls", 2))
			{
				/* FIXME: This should be a random port */
				int listenfd = init_data_connection (connfd, 1024 + rand());
				char buf[MAXLINE];

				sprintf (buf, "LIST %s", args);
				sock_print_nostat (connfd, buf);

				/* FIXME: WTF? Check what the server thinks about this */
				our_readline (buf, connfd);
				if (response_to_status (buf) != FTP_FILE_STATUS_OK_OPEN_CONN)
					printf ("[DEBUG] Status: %u\n", response_to_status (buf));
				data_transfer (listenfd, connfd, 1, buf);
				our_readline (buf, connfd);
				if (response_to_status (buf) != FTP_FINISHED_CLOSING)
					printf ("[DEBUG] Status: %u\n", response_to_status (buf));
			}
      else if (!strncmp (cmd, "lpwd", 4))
			{
				char buf[MAXLINE];
				getcwd (buf, MAXLINE);
				printf ("%s\n", buf);
		  }
			else if (!strncmp (cmd, "lcd", 3))
			{
				chdir (args);
			}
			else if (!strncmp (cmd, "lls", 3))
			{
				/* This is a SNEAKY hack ... */
				system (line+1);
			}
			else if (!strncmp (cmd, "pwd", 3))
			{
				sock_print_nostat (connfd, "PWD");
				char buf[MAXLINE];
				our_readline (buf, connfd);
				if (FTP_CMD_OK == response_to_status (buf))
				{
					printf ("%s\n", buf+4);
				}
			}
			else if (!strncmp (cmd, "cd", 2))
			{
				char buf[MAXLINE];
				sprintf (buf, "CWD %s", args);

				sock_print_nostat (connfd, buf);
				our_readline (buf, connfd);
				if (FTP_CMD_OK != response_to_status (buf))
					printf ("%s\n", buf+4);
			}
			else if (!strncmp (cmd, "get", 3))
			{
				int listenfd = init_data_connection (connfd, 1024 + rand ());
				char buf[MAXLINE];
				char args[MAXPATH];
				/* We can do the +4 stuff, as we know the command was "get" */
				sprintf (buf, "RETR %s", line+4);
				sprintf (args, "local-%s", line+4);
				sock_print_nostat (connfd, buf);

				int f = open (args,
						O_CREAT | O_WRONLY,
						S_IRWXU | S_IRWXG | S_IROTH);

				if (f < 0)
					perror ("");
				#ifdef DEBUG
					printf ("[DEBUG] Opened local file %s\n", args);
				#endif

				/* Check what the server thinks about this */
				our_readline (buf, connfd);
				#ifdef DEBUG
					if (response_to_status (buf) != FTP_FILE_STATUS_OK_OPEN_CONN)
						printf ("[DEBUG] Status: %u\n", response_to_status (buf));

					printf ("[DEBUG] Starting datatransfer\n");
				#endif
				data_transfer (listenfd, connfd, f, buf);
				close (f);
				our_readline (buf, connfd);
				if (response_to_status (buf) == FTP_FINISHED_CLOSING)
					close (listenfd);
			}
			else if (!strncmp (cmd, "put", 4))
			{
				int listenfd = init_data_connection (connfd, 1024 + rand ());
				char buf[MAXLINE];

				char args[MAXPATH];
				/* We can do the +4 stuff, as we know the command was "put" */
				sprintf (buf, "STOR remote-%s", line+4);
				sprintf (args, "%s", line+4);
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
			else if (!strncmp (cmd, "help", 4))
			{
				printf ("Available commands: {ls,pwd,cd,put,get, lls,lcd,pwd,logout}\n");
			}

			else
				printf ("Unknown command: %s\n", cmd);
		}
	}

	/* Clean up our memory */
	history_end(myhistory);
	el_end(el);
	return 0;
}
