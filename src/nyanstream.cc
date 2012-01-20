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
    si_client.sin_port = htons(1338);
    
    sendto(sock1, "bonjour", 8, 0, (sockaddr*)&si_client, result ->ai_addrlen);

    close(sock1);
    
    freeaddrinfo(result);
}

void printUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "nyanstream <-s/-c> <address>" << std::endl;
    std::cout << "s: server, c: client" << std::endl;
    std::cout << "address: address of the remote end" << std::endl;
}

}

int main( int argc, char* argv[]) 
{
    if (argc < 3)
    {
        nyanstream::printUsage();
        return EXIT_SUCCESS;
    }
    
    if (strcmp(argv[1], "-s") == 0) {
        return nyanstream::server(argv);
    }
    
    return EXIT_SUCCESS;
    
}
