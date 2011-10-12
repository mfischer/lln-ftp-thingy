/**
@file handlers.c
@brief This file contains the definition of all the command implementted by the server.
@author Moritz FISCHER & Thibault MERLE
@version 1.0
@date 10-11-2011
**/
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "handlers.h"
#include "constants.h"
#include "utils.h"

/**
@fn void syst_handler (int connfd)
@brief This function sends ... .
@param connfd is a socket descriptor where we write the message code.
**/
void syst_handler (int connfd)
{
	sock_print (connfd, FTP_SYST, FTP_SYST_RESP_STR);
	printf ("[DEBUG] SYST handler\n");
}

/**
@fn void port_handler (int connfd, void* cmdptr, int* datafd)
@brief This function connects to a client.
@param connfd is a socket descriptor where we write our message.
@param cmpdtr is a structure which contains the prot of the client and its address.
@param datafd is a new descriptor with the etablishment of the data connection.
**/
void port_handler (int connfd, void* cmdptr, int* datafd)
{
	printf ("[DEBUG] PORT handler\n");
	cmd_port_t* tmp = (cmd_port_t*) cmdptr;

	struct sockaddr_in servaddr;
	memset ((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(tmp->addr);
	servaddr.sin_port        = htons(tmp->port);

	printf ("[DEBUG] Server trying to connect to %s %u\n", tmp->addr, tmp->port);
	if ( (*datafd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		/*FIXME: Don't exit here */
		exit_error ("Socket initialization error");

	if (connect (*datafd , (struct sockaddr *) &servaddr , sizeof ( servaddr )) < 0)
	{
		exit_error ("connect error");
		sock_print (connfd, FTP_CANT_OPEN_DATA_CONN, FTP_CANT_OPEN_DATA_CONN_STR);
	}

	sock_print (connfd, FTP_CMD_OK, FTP_CMD_OK_STR);
}

/**
@fn void list_handler (int datafd, int connfd)
@brief This function is the command "ls" executed on the server.
@param datafd is ... .
@param connfd is a socket descriptor of the client where data are sent.
**/
void list_handler (int datafd, int connfd)
{
	printf ("[DEBUG] LIST handler\n");
	sock_print (connfd, FTP_FILE_STATUS_OK_OPEN_CONN, FTP_FILE_STATUS_OK_OPEN_CONN_STR);
	/* FIXME make this NAME_MAX */
	char cwd[MAXPATH];
	DIR* dirp = opendir (getcwd (cwd, MAXPATH));
	struct dirent* cur;
	while (NULL != (cur = readdir(dirp)))
	{
		/* Here we reuse the cwd buffer to add the <CRLF>*/
    snprintf(cwd, MAXPATH, "%s\r", cur->d_name);
		sock_print_nostat (datafd, cwd);
	}

	close (datafd);
	/* Note: We do NOT have to free(3) dirp, as per manpage it's statically allocated */
	sock_print (connfd, FTP_FINISHED_CLOSING, FTP_FINISHED_CLOSING_STR);
}

/**
@fn void pwd_handler (int datafd, int connfd)
@brief This function handles the PWD request.
@param connfd is the socket descriptor for the control connection.
**/
void pwd_handler (int connfd)
{
	printf ("[DEBUG] PWD handler\n");
	char tmp[MAXLINE];
    getcwd (tmp, (size_t) MAXLINE);	
	sock_print (connfd, FTP_CMD_OK, tmp);
}

/**
@fn void quit_handler (int datafd, int connfd)
@brief This function handles the QUIT request, by closing the control connection.
@param connfd is the socket descriptor for the control connection.
**/
void quit_handler (int connfd)
{
	printf ("[DEBUG/TODO] QUIT handler\n");
	sock_print(connfd, FTP_BYE, FTP_BYE_STR);
	close (connfd);
	_exit (0);
}

/**
@fn void stor_handler (int datafd, void* cmdptr, int connfd)
@brief This function is the command "put" executed on the server. A file is sent to the server.
@param datafd is ... .
@param cmdptr is a structure containing the path of the file.
@param connfd is a socket descriptor of the client where data are sent.
**/
void stor_handler (int datafd, void* cmdptr, int connfd)
{
	cmd_stor_t* tmp = (cmd_stor_t *) cmdptr;
	sock_print (connfd, FTP_FILE_STATUS_OK_OPEN_CONN, FTP_FILE_STATUS_OK_OPEN_CONN_STR);

	printf ("[DEBUG] Trying to save to path: %s\n", tmp->path);
	char buf[MAXLINE];

	int fd = open (tmp->path,
			           O_CREAT | O_WRONLY, 
	               S_IRWXU | S_IRWXG | S_IROTH);
	if (fd <0)
	{
		printf ("[ERROR] Something went wrong\n");
		sock_print (connfd, FTP_ACCESS_DENIED, "ERROR ACCESS DENIED");
		return;
	}

	ssize_t len;
	for(;;)
	{
		len = read (datafd, (void *) buf, MAXLINE * sizeof(char));
		write (fd, (const void*) buf, len * sizeof(char));
		if (len == 0)
		{
			close (fd);
			sock_print (connfd, FTP_FINISHED_CLOSING, FTP_FINISHED_CLOSING_STR);
			return;
		}
	}
}

/**
@fn void stor_handler (int datafd, void* cmdptr, int connfd)
@brief This function is the command "get" executed on the server. A file is sent to the client.
@param datafd is ... .
@param cmdptr is a structure containing the path of the file.
@param connfd is a socket descriptor of the client where data are sent.
**/
void retr_handler (int datafd, void* cmdptr, int connfd)
{
	printf ("[DEBUG/TODO] RETR handler\n");
	char buf[MAXLINE];
	cmd_cwd_t* tmp = (cmd_cwd_t *) cmdptr;
	sock_print (connfd, FTP_FILE_STATUS_OK_OPEN_CONN, FTP_FILE_STATUS_OK_OPEN_CONN_STR);
	int fd = open (tmp->path,
			           O_RDONLY);
	if (fd <0)
	{
		sock_print (connfd, FTP_ACCESS_DENIED, "Access denied.");
		return;
	}
	ssize_t len;
	for(;;)
	{
		len = read (fd, (void *) buf, MAXLINE * sizeof(char));
		write (datafd, (const void*) buf, len * sizeof(char));
		if (len == 0)
		{
			close (fd);
			close (datafd);
			sock_print (connfd, FTP_FINISHED_CLOSING, FTP_FINISHED_CLOSING_STR);
			return;
		}
	}

}

/**
@fn void cwd_handler (int datafd, void* cmdptr, int connfd)
@brief This function handles the CWD command.
@param cmdptr is a structure containing the parameters for the CWD cmd (path to change to).
@param connfd is the socket descriptor for the control connection.
**/
void cwd_handler (void* cmdptr, int connfd)
{
	printf ("[DEBUG] CWD handler\n");

	cmd_cwd_t* tmp = (cmd_cwd_t *) cmdptr;
	sock_print (connfd, FTP_CMD_OK, FTP_CMD_OK_STR);
	/* FIXME Here we have to check WHERE we're changing to! */
  	chdir (tmp->path);
}

/**
@fn void not_implemented_handler (int connfd)
@brief This function sends to the client that the command given is not implemented. So it returns an error. 
@param connfd is a socket descriptor of the client where data are sent.
**/
void not_implemented_handler (int connfd)
{
	printf ("[DEBUG]: NOT_IMPLEMENTED_HANDLER handler\n");
	sock_print (connfd, FTP_COMMAND_NOT_IMPLEMENTED, "");
}
