#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVER_PORT  7000
#define SERVER_ADDR "130.104.230.234"

int main(argc, argv) int    argc; char   *argv[ ];
{
    int sd1;
    struct sockaddr_in server;

    /* fill in the structure "m2" with the address of the
     * server that we want to connect with */
    memset((char *) &server, 0, sizeof(server));
    server.sin_family     = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server.sin_port       = htons(SERVER_PORT);

    /* Open a TCP socket (an Internet stream socket)*/
    if ( (sd1 = socket(PF_INET, SERVER_PORT, 0)) < 0){
        perror("socket error in telnet");
        exit(-1);
    }

    /* Connect to the server */
    if (connect(sd1 , (struct sockaddr *) &server , sizeof ( server )) < 0){
        perror("connect error in telnet");
        exit(-1);
    }
    /*client code */
    return 0;
}

	

