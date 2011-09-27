#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

const uint16_t CMD_PORT  = 7000;
const uint16_t DATA_PORT = 7001;
const int      BACKLOG   = 5;

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
handle_sig_chld (int signo)
{
	return;
}

void
handle_client_connection (int* connfd)
{
	printf ("Client connected!\n");
	return;
}

int
main (int argc, char** argv)
{
	(void) argc;
	(void) argv;
	int listenfd, connfd;

	/* Set up the stuff for the listenfd */
	struct sockaddr_in servaddr, clientaddr;
	memset(&servaddr, 0, sizeof (struct sockaddr_in));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (7000);

	listenfd = socket (AF_INET, SOCK_STREAM, 0);

	print_header ();
	
	/* We try to bind to port 7000 on any address */
	int info;
	if ( ((info = bind (listenfd, (const struct sockaddr*) &servaddr, sizeof (servaddr)))))
	{
		perror ("Error with bind: ");
	}

	/* We mark the socket as a passive, i.e. listening socket */
	if ( ((info = listen (listenfd, BACKLOG)) < 0) )
	{
		perror ("Error with listen: ");
	}

	/* Next we set up the signal handler for SIGCHLD */
	/*struct sigaction sigact;*/
	/*sigact.sa_handler = handle_sig_chld;*/
	/*(void) sigact;*/

	/* FIXME complete this stuff */

	/* We accept () to obtain a connected socket */
	socklen_t clientlen = (socklen_t) sizeof (clientaddr);
	pid_t pid;

	connfd = accept (listenfd, (struct sockaddr*) &clientaddr, &clientlen);
	/*while (1)*/
	/*{*/

		/*[> We're in the child <]*/
		/*if ( (pid = fork ()) == 0 )*/
		/*{*/
			/*printf ("Started child with pid: %u!\n", pid);*/
			/*close (listenfd);*/
			/*handle_client_connection (&connfd);*/
			/*close (connfd);*/
			/*exit (EXIT_SUCCESS);*/
		/*}*/

		/*[> Parent code <]*/
		/*char client_str[INET_ADDRSTRLEN];*/
		/*socklen_t cliaddr_len = sizeof (struct in_addr);*/
		/*inet_ntop (AF_INET, (const void*) &clientaddr.sin_addr, client_str, cliaddr_len);*/
		/*close (connfd);*/
	/*}*/
	(void) connfd;
	(void) pid;
	
	return EXIT_SUCCESS;
}
