/**
@file utils.c
@brief This file contains functions which is used both by the server and the client.
@author Moritz FISCHER & Thibault MERLE
@version 1.0
@date 10-11-2011
**/
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "constants.h"

/**
@fn void exit_error (const char *err_msg)
@brief This function displays the error message and exits the program.
@param err_msg is a string that contains the error message.
**/
void exit_error (const char *err_msg)
{
	printf ("Error: %s\n", err_msg);
	exit (EXIT_FAILURE);
}

/**
@fn inline unsigned int get_client_port (const unsigned int p1, const unsigned int p2)
@brief This function gets the client port number from the port p1 and p2. It's in the protocol ftp85.
@param p1 is a port number.
@param p2 is another port number.
@return Returns the port number of the client.
**/
inline 
unsigned int get_client_port (const unsigned int p1,
		             				const unsigned int p2)
{
	return p1*256+p2;
}

/**
@fn void generate_client_ports (unsigned int* p1, unsigned int* p2, unsigned int p)
@brief This function generates following the protocol ftp85 p1 and p2.
@param p1 is a pointor that will be filled.
@param p2 is a pointor that will be filled.
@param p is the port number of the client.
**/
void generate_client_ports (unsigned int* p1, unsigned int* p2, unsigned int p)
{
	*p1 = p / 256;
	*p2 = p % 256;
}

/**
@fn size_t our_readline (char* readbuf, int connfd)
@brief This function reads data of a line from a descriptor and put the data read into the buffer readbuf.
@param readbuf is a buffer that contains data.
@param connfd is a descriptor of socket.
@return Returns the number of characters read.
**/
size_t our_readline (char* readbuf, int connfd)
{
	size_t readcnt = 0;
	memset (readbuf, 0, sizeof (readbuf));
	while (readcnt < MAXLINE)
	{
		read (connfd, (void*) readbuf+readcnt, sizeof(char));
		if (readbuf[readcnt ] == '\n')
		{
			readbuf[readcnt ] = '\0';
			break;
		}
		else
			readcnt++;
	}
	return readcnt;
}

/**
@fn void sock_print (int fd, uint16_t code, char* str)
@brief This function gets the status of a request
@param s is a string that contains the return code.
@return Returns the status of the request into an unsigned int
**/
void sock_print (int fd, uint16_t code, char* str)
{
	char buffer[MAXLINE + 1];
	memset (buffer, 0, sizeof (buffer));
	snprintf (buffer, sizeof (buffer), "%u %s\n", code, str);
	write (fd, (const void*) buffer, strlen (buffer));
}

/**
@fn void sock_print_nostat (int fd, char* str)
@brief This function gets the status of a request
@param s is a string that contains the return code.
@return Returns the status of the request into an unsigned int
**/
void sock_print_nostat (int fd, char* str)
{
	char buffer[MAXLINE + 1];
	memset (buffer, 0, sizeof (buffer));
	snprintf (buffer, sizeof (buffer), "%s\n", str);
	write (fd, (const void*) buffer, strlen (buffer));
}

/**
@fn unsigned int line_to_cmd (const char* line, size_t readcnt, void* cmd)
@brief This function gets the status of a request
@param s is a string that contains the return code.
@return Returns the status of the request into an unsigned int
**/
unsigned int line_to_cmd (const char* line, size_t readcnt, void* cmd)
{
	if (readcnt < MAXCMD-1)
	{
		printf ("Parse error in: %s\n", line);
		return FTP_CMD_PARSE_ERROR;
	}

	char cur_cmd[MAXCMD];
	sscanf (line, "%s", cur_cmd);

	if (!strncmp (cur_cmd, "USER", MAXCMD))
	{
		cmd_user_t* tmp = (cmd_user_t*) cmd;
		sscanf (line, "%s %s", cur_cmd, &tmp->user[0]);
		return FTP_CMD_USER;
	}
	else if (!strncmp (cur_cmd, "PASS", MAXCMD))
	{
		cmd_pass_t* tmp = (cmd_pass_t*) cmd;
		sscanf (line, "%s %s", cur_cmd, &tmp->pass[0]);
		return FTP_CMD_PASS;
	}
	else if (!strncmp (cur_cmd, "PASV", MAXCMD))
	{
		return FTP_CMD_PASV;
	}
	else if (!strncmp (cur_cmd, "PORT", MAXCMD))
	{
		cmd_port_t* tmp = (cmd_port_t*) cmd;
		unsigned int h1, h2, h3, hMAXCMD, p1, p2;
		sscanf (line, "%s %u,%u,%u,%u,%u,%u",
				    cur_cmd, &h1, &h2, &h3, &hMAXCMD, &p1, &p2);
		tmp->port = get_client_port(p1,p2);
		sprintf (&tmp->addr[0], "%u.%u.%u.%u",h1,h2,h3,hMAXCMD);
		return FTP_CMD_PORT;
	}
	else if (!strncmp (cur_cmd, "SYST", MAXCMD))
	{
		return FTP_CMD_SYST;
	}
  else if (!strncmp (cur_cmd, "LIST", MAXCMD))
	{
		return FTP_CMD_LIST;
	}
	else if (!strncmp (cur_cmd, "PWD", MAXCMD))
	{
		return FTP_CMD_PWD;
	}
	else if (!strncmp (cur_cmd, "QUIT", MAXCMD))
	{
		return FTP_CMD_QUIT;
	}
	else if (!strncmp (cur_cmd, "STOR", MAXCMD))
	{
		cmd_stor_t* tmp = (cmd_stor_t*) cmd;
		sscanf(line, "%s %s", cur_cmd, &tmp->path[0]);
		return FTP_CMD_STOR;
	}
	else if (!strncmp (cur_cmd, "RETR", MAXCMD))
	{
		cmd_retr_t* tmp = (cmd_retr_t*) cmd;
		sscanf(line, "%s %s", cur_cmd, &tmp->path[0]);
		return FTP_CMD_RETR;
	}

	else if (!strncmp (cur_cmd, "CWD", MAXCMD))
	{
		cmd_cwd_t* tmp = (cmd_cwd_t*) cmd;
		sscanf(line, "%s %s", cur_cmd, &tmp->path[0]);
		return FTP_CMD_CWD;
	}
	else
	{
		printf ("Shit we don't know \"%s\" yet\n", cur_cmd);
		return 0xff;
	}
}
