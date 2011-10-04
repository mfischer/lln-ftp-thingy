#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

void
exit_error (const char *err_msg)
{
	printf ("Error: %s\n", err_msg);
	exit (EXIT_FAILURE);
}

inline unsigned int
get_client_port (const unsigned int p1,
		             const unsigned int p2)
{
	return p1*256+p2;
}

unsigned int
line_to_cmd (const char* line, size_t readcnt, void* cmd)
{
	if (readcnt < MAXCMD)
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

	return 0xff;
}
