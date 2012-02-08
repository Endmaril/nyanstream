#include "server.hh"
#include "client.hh"

namespace nyanstream
{

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
        nyanstream::Client client(argv);
        return client.run();
    }
    
    return EXIT_SUCCESS;
    
}
