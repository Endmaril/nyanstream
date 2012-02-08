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

int main(int argc, char* argv[]) 
{
    if (argc < 4)
    {
        nyanstream::printUsage();
        return EXIT_SUCCESS;
    }

    SDL_Init(SDL_INIT_AUDIO);
    
    int res;

    if (strcmp(argv[1], "-s") == 0) {
        nyanstream::Server server(argv);
        res = server.run();
    } else if (strcmp(argv[1], "-c") == 0) {
        res = nyanstream::client(argv);
    }
    
    return res;
    
}
