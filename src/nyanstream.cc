#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

namespace nyanstream
{

int main( int argc, char* argv[]) 
{
    if (argc < 4)
    {
        printUsage();
        return EXIT_SUCCESS;
    }
    
    if (strcmp(argc[1], "-s") == 0) {
        return server(argv);
    }
    
    return EXIT_SUCCESS;
    
}

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
    
    if (bind(sock1, &si_serv, sizeof(si_serv))==-1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }
    
    struct addrinfo *result;
    
    int error = getaddrinfo(argv[2], NULL, NULL, &result);
    int (error != 0)
    {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    
    sendto(sock1, "bonjour", 8, 0, result->ai_addr, result ->ai_addrlen);
    
    close(sock1);
    
    freeaddrinfo(result);
}

void printUsage()
{
    cout << "Usage:" << endl;
    cout << "nyanstream <-s/-c> <address>" << endl;
    cout << "s: server, c: client" << endl;
    cout << "address: address of the remote end" << endl;
}

}
