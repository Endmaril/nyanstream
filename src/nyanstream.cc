#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace nyanstream
{

int server(char* argv[])
{
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock1 == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in si_serv;
    
    memset((char *) &si_serv, 0, sizeof(si_serv));
    si_serv.sin_family = AF_INET;
    si_serv.sin_port = htons(1337);
    si_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock1, (sockaddr*)&si_serv, sizeof(si_serv))==-1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }
    
    struct addrinfo *result;
    
    int error = getaddrinfo(argv[2], NULL, NULL, &result);
    if(error != 0)
    {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in si_client;
    memcpy(&si_client, result -> ai_addr, sizeof(si_client));    
    int port = strtol(argv[3], (char **) NULL, 10);
    
    si_client.sin_port = htons(port);
    
    sendto(sock1, "bonjour", 8, 0, (sockaddr*)&si_client, result ->ai_addrlen);

    close(sock1);
    
    freeaddrinfo(result);
}

int client(char* argv[])
{
    int sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock1 == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in si_serv;
    
    memset((char *) &si_serv, 0, sizeof(si_serv));
    si_serv.sin_family = AF_INET;
    int port = strtol(argv[3], (char **) NULL, 10);
    si_serv.sin_port = htons(port);
    si_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock1, (sockaddr*)&si_serv, sizeof(si_serv))==-1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }
    
    struct sockaddr exp;
    unsigned int lgdest = sizeof(sockaddr);
    char msg[10];
    
    int numDataRecv = recvfrom(
        sock1,              /* descripteur de la socket de reception */
        msg,               /* adresse de recuperation du message    */
        10,                 /* taille de l'espace alloue a msg       */
        0,                  /* toujours 0 pour le type SOCK_DGRAM    */
        &exp,                /* pour recuperer l'adr de l'expediteur  */
        &lgdest             /* taille espace reserve a dest          */
    );
    
    std::cout << numDataRecv << std::endl;

    close(sock1);
    
}

void printUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "nyanstream <-s/-c> <address> <port>" << std::endl;
    std::cout << "port: port on which data must be sent if it is the server or on which we listen for the client" << std::endl;
    std::cout << "s: server, c: client" << std::endl;
    std::cout << "address: address of the remote end" << std::endl;
}

}

int main( int argc, char* argv[]) 
{
    if (argc < 4)
    {
        nyanstream::printUsage();
        return EXIT_SUCCESS;
    }
    
    if (strcmp(argv[1], "-s") == 0) {
        return nyanstream::server(argv);
    } else if (strcmp(argv[1], "-c") == 0) {
        return nyanstream::client(argv);
    }
    
    return EXIT_SUCCESS;
    
}
