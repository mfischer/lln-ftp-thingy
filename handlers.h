#ifndef HANDLERS_H
#define HANDLERS_H

void syst_handler (int connfd);
void port_handler (int connfd, void* cmdptr, int* datafd);
void list_handler (int datafd, int connfd);
void pwd_handler (int datafd, int connfd);
void not_implemented_handler (int connfd);
void quit_handler (int datafd, int connfd);
void stor_handler (int datafd, void* cmdptr, int connfd);
void retr_handler (int datafd, void* cmdptr, int connfd);
void cwd_handler (int datafd, void* cmdptr, int connfd);


#endif /* HANDLERS_H */