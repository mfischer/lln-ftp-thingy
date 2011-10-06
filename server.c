#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "constants.h"
#include "utils.h"
#include "handlers.h"

void
print_header (void)
{
	printf ("Trying to start server with pid %u on %s:%u ...\n", getpid (), "0.0.0.0", CMD_PORT);
}

void
handle_client_connection (int connfd)
{
	char readbuf[MAXLINE + 1];

	/* We print the ready header */
	sock_print (connfd, FTP_READY, FTP_READY_STR);

	size_t len;
	void* cmdptr = malloc (sizeof(union cmd));
	int datafd;

	unsigned int current_cmd;

	/* We wait for a username */
	while (1)
	{
  	len	= our_readline (readbuf, connfd);
		current_cmd = line_to_cmd (readbuf, len, cmdptr);
		if (FTP_CMD_USER == current_cmd)
		{
			cmd_user_t cmd = *((cmd_user_t*) cmdptr);
			printf ("Ok, got username %s\n", cmd.user);
			sock_print (connfd, FTP_USER_OK, FTP_USER_OK_STR);
			break;
		}
	}

	/* We wait for a password */
	while (1)
	{
		len	= our_readline (readbuf, connfd);
		if (FTP_CMD_PASS == line_to_cmd (readbuf, len, cmdptr))
		{
			cmd_pass_t cmd = *((cmd_pass_t*) cmdptr);
			printf ("Ok, got pass %s\n", cmd.pass);
			sock_print (connfd, FTP_USER_LOGGED_IN, "");
			break;
		}
		sock_print (connfd, FTP_USER_OK, FTP_USER_OK_STR);
  }

	/* Our mainloop ...*/
	while (1)
	{
		len = our_readline (readbuf, connfd);
		switch (line_to_cmd (readbuf, len, cmdptr))
		{
			case FTP_CMD_SYST:
				syst_handler (connfd);
				break;

			case FTP_CMD_PORT:
				port_handler (connfd, cmdptr, &datafd);
				break;

			case FTP_CMD_LIST:
				list_handler (datafd, connfd);
				break;

			case FTP_CMD_PWD:
				pwd_handler (datafd, connfd);
				break;

			case FTP_CMD_CWD:
				cwd_handler (datafd, cmdptr, connfd);
				break;

			case FTP_CMD_QUIT:
				quit_handler (datafd, connfd);
				break;

			case FTP_CMD_RETR:
				retr_handler (datafd, cmdptr, connfd);
				break;

			case FTP_CMD_STOR:
				stor_handler (datafd, cmdptr, connfd);
				break;

			default:
				not_implemented_handler (connfd);
				break;
		}
	}

	close (datafd);
	free (cmdptr);
	return;
}

int
main (int argc, char** argv)
{
	/* Shut up the compiler ... */
	(void) argc;
	(void) argv;
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in clientaddr;

	listenfd = socket (AF_INET, SOCK_STREAM, 0);

	/* Set up the stuff for the listenfd */
	memset (&servaddr, 0, sizeof (struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (CMD_PORT);

	/* We try to bind to port 7000 on any address */
	int info = 0;
	info = bind (listenfd, (const struct sockaddr*) &servaddr, sizeof (servaddr));
	if (info < 0)
		exit_error("Something went wrong with bind");

	/* We mark the socket as a passive, i.e. listening socket */
	info = listen (listenfd, BACKLOG);
	if (info < 0)
		exit_error("Something went wrong with listen");

	/* FIXME: Apparently this is NOT portable */
	signal (SIGCHLD, SIG_IGN);


	/* We accept () to obtain a connected socket */
	socklen_t clientlen = (socklen_t) sizeof (clientaddr);
	pid_t pid;

	/* Our mainloop goes on forever ... */
	while (1)
	{
		connfd = accept (listenfd, (struct sockaddr*) &clientaddr, &clientlen);
		/* We're in the child */

		if ( (pid = fork ()) == 0 )
				{
					printf ("Started child with pid: %u!\n", getpid());
					close (listenfd);
					handle_client_connection (connfd);
					close (connfd);
					printf ("Terminating child with pid: %u!\n", getpid());
					exit (EXIT_SUCCESS);
				}
		/* Parent code: FIXME do something here ;-) */
		close (connfd);
	}

	close (listenfd);
	return EXIT_SUCCESS;
}
