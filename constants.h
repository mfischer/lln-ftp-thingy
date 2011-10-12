/**
@file constants.h
@brief This file contains the constants used in the application.
@author Moritz FISCHER & Thibault MERLE
@version 1.0
@date 10-11-2011
**/
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define FTP_READY 220
#define FTP_READY_STR "Welcome, FTP Server ready."

#define FTP_BYE 221
#define FTP_BYE_STR "KthxBai"

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

#define FTP_FINISHED_CLOSING 226
#define FTP_FINISHED_CLOSING_STR "We're done, closing data connection."

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

#define FTP_ACCESS_DENIED 550
#define FTP_ACCESS_DENIED_STR "Access denied -> FIXME"

#define MAXLINE   255

#define FTP_CMD_USER 0x01
#define FTP_CMD_PASS 0x02
#define FTP_CMD_PORT 0x03
#define FTP_CMD_PASV 0x04
#define FTP_CMD_SYST 0x05
#define FTP_CMD_LIST 0x06
#define FTP_CMD_PWD  0x07
#define FTP_CMD_QUIT 0x08
#define FTP_CMD_RETR 0x09
#define FTP_CMD_STOR 0x0A
#define FTP_CMD_CWD  0x0B
#define FTP_CMD_PARSE_ERROR 0xff;


#define CMD_PORT  7000
#define DATA_PORT 7001
#define BACKLOG   100
#define MAXLINE   255

#define MAXCMD 4
#define MAXIPLEN 20

#define MAXUSER 255
#define MAXPASS 255

#define MAXPATH 255


/**
@union cmd 
@brief This union is used to store the different possible commands including their parameters.
**/
union cmd
{
	/**
	@struct cmd_user
	@brief USER command containing the username.
	**/	
	struct cmd_user
	{
		char user[MAXUSER];
  	} cmd_user;
	
	/**
	@struct cmd_pass
	@brief PASS command containing the password.
	**/	
	struct cmd_pass
	{
		char pass[MAXUSER];
  	} cmd_pass;
	
	/**
	@struct cmd_port
	@brief PORT command containing the port number and IP address.
	**/	
	struct cmd_port
	{
		unsigned int port;
		char addr[MAXIPLEN];
  	} cmd_port;
	
	/**
	@struct cmd_stor
	@brief STOR command containing information where to store the file.
	**/	
	struct cmd_stor
	{
		char path[MAXPATH];
	} cmd_stor;
	
	/**
	@struct cmd_retr
	@brief RETR command containing information about the path of the file to retrieve.
	**/	
	struct cmd_retr
	{
		char path[MAXPATH];
	} cmd_retr;
	
	/**
	@struct cmd_cwd
	@brief CWD command containing the path to change to.
	**/	
	struct cmd_cwd
	{
		char path[MAXPATH];
	} cmd_cwd;

};

typedef struct cmd_user cmd_user_t;
typedef struct cmd_pass cmd_pass_t;
typedef struct cmd_port cmd_port_t;
typedef struct cmd_stor cmd_stor_t;
typedef struct cmd_retr cmd_retr_t;
typedef struct cmd_cwd cmd_cwd_t;

#endif /* CONSTANTS_H */
