#ifndef CODES_H
#define CODES_H

#define FTP_READY 220
#define FTP_READY_STR "Welcome, FTP Server ready."

#define FTP_CMD_OK 200
#define FTP_CMD_OK_STR "Command ok."

#define FTP_USER_OK 331
#define FTP_USER_OK_STR ""

#define FTP_USER_LOGGED_IN 230
#define FTP_USER_LOGGED_IN_STR "Welcome!"

#define FTP_SYST 215 
#define FTP_SYST_RESP_STR "UNIX unknown"

#define FTP_ACCEPT_PASV 227

#define FTP_ACCEPT_PORT 200
#define FTP_ACCEPT_PORT_STR "PORT Command accepted"

#define FTP_DIRECTORY_STATUS 212
#define FTP_FILE_STATUS 213

#define FTP_COMMAND_NOT_IMPLEMENTED 202

#define FTP_NEED_LOGIN 332
#define FTP_NEED_LOGIN_STR "You need to login before proceeding"

#define FTP_NOT_LOGGED_IN 530
#define FTP_NOT_LOGGED_IN_STR "You're not logged in"

#define FTP_FILE_STATUS_OK_OPEN_CONN 150
#define FTP_FILE_STATUS_OK_OPEN_CONN_STR "File status okay, about to open data connection."

#define FTP_CANT_OPEN_DATA_CONN 425
#define FTP_CANT_OPEN_DATA_CONN_STR "Can't open data connection."

#define FTP_REQUESTED_ACTION_FINISHED 250
#define FTP_REQUESTED_ACTION_FINISHED_STR "Requested file action okay, completed."

#define MAXLINE   255

#define FTP_CMD_USER 0x01
#define FTP_CMD_PASS 0x02
#define FTP_CMD_PORT 0x03
#define FTP_CMD_PASV 0x04
#define FTP_CMD_SYST 0x05
#define FTP_CMD_LIST 0x06
#define FTP_CMD_PARSE_ERROR 0xff;


#define CMD_PORT  7000
#define DATA_PORT 7001
#define BACKLOG   100
#define MAXLINE   255

#define MAXCMD 4
#define MAXIPLEN 20

#define MAXUSER 255
#define MAXPASS 255



union cmd
{
	struct cmd_user
	{
		char user[MAXUSER];
  } cmd_user;
	struct cmd_pass
	{
		char pass[MAXUSER];
  } cmd_pass;
	struct cmd_port
	{
		unsigned int port;
		char addr[MAXIPLEN];
  } cmd_port;
};

typedef struct cmd_user cmd_user_t;
typedef struct cmd_pass cmd_pass_t;
typedef struct cmd_port cmd_port_t;


#endif /* CODES_H */
