#include <sstream>
#include "common.hh"

namespace nyanstream
{

class Server
{
private:
    int socketServer;
    std::string addressServer;
    struct sockaddr_in siServer;
    uint16_t serverPort;
    struct sockaddr sourceAddress;

    SDL_AudioSpec asNyan;

    bool recvNegociation();
public:
    Server(char* argv[]);
    ~Server();

    int run();
};

}
