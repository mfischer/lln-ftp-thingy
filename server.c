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


const uint16_t CMD_PORT  = 7000;
const uint16_t DATA_PORT = 7001;
const int      BACKLOG   = 100;

void
print_header (void)
{
	printf ("Trying to start server with pid %u on %s:%u ...\n", getpid (), "0.0.0.0", CMD_PORT);
}

void
exit_error (const char *err_msg)
{
	printf ("Error: %s\n", err_msg);
	exit (EXIT_FAILURE);
}

void
handle_client_connection (int* connfd)
{
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
					handle_client_connection (&connfd);
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
