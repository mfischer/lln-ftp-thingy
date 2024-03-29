/**
@file utils.h
@brief This file contains functions which are used both by the server and the client.
@author Moritz FISCHER & Thibault MERLE
@version 1.0
@date 10-11-2011
**/
#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>

void exit_error (const char *err_msg);
unsigned int line_to_cmd (const char* line, size_t readcnt, void* cmd);
unsigned int get_client_port (const unsigned int p1, const unsigned int p2);
void sock_print (int fd, uint16_t code, char* str);
void sock_print_nostat (int fd, char* str);
size_t our_readline (char* readbuf, int connfd);
void generate_client_ports (unsigned int* p1, unsigned int* p2, unsigned int p);

#endif /* UTILS_H */
