#ifndef UTILS_H
#define UTILS_H

void exit_error (const char *err_msg);
unsigned int line_to_cmd (const char* line, size_t readcnt, void* cmd);
unsigned int get_client_port (const unsigned int p1, const unsigned int p2);


#endif /* UTILS_H */
