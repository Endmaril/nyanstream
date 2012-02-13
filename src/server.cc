#include "server.hh"

namespace nyanstream
{

Server::Server(char* argv[3])
{
    addressServer = argv[2];
    serverPort = strtol(argv[3], (char**)NULL, 10);
}

Server::~Server()
{
}

int Server::run()
{
    socketServer = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketServer == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }
    
    // listen   
    memset((char *) &siServer, 0, sizeof(siServer));
    siServer.sin_family = AF_INET;
    siServer.sin_port = htons(serverPort);
    siServer.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(socketServer, (sockaddr*)&siServer, sizeof(siServer))==-1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }
    
    struct addrinfo *result;
    
    int error = getaddrinfo(addressServer.c_str() , NULL, NULL, &result);
    if(error != 0)
    {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    
    // read sound
    SDL_AudioCVT cvt;
    Uint8* data;
    Uint32 dlen;
    if(SDL_LoadWAV("data/nyan.wav", &asNyan, &data, &dlen) == NULL)
    {
        fprintf(stderr, "SDL_LoadWAV(%s): %s\n", "data/nyan.wav", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_BuildAudioCVT(&cvt, asNyan.format, asNyan.channels, asNyan.freq, asNyan.format, 1, asNyan.freq);
    cvt.buf = (Uint8*)malloc(dlen * cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_ConvertAudio(&cvt);

    std::cout << "Wav is " << dlen << " long" << std::endl;

    while(!recvNegociation());

    Uint32 sample_size;
    switch(asNyan.format)
    {
    case AUDIO_U8:
    case AUDIO_S8:
        sample_size = 1;
        break;
    case AUDIO_U16LSB:
    case AUDIO_S16LSB:
    case AUDIO_U16MSB:
    case AUDIO_S16MSB:
        sample_size = 2;
        break;
    }
    float delay = 1.0f / (2.0f * (float)asNyan.freq / (float)NYAN_BUFFER_SIZE);
    for(int i = 0; i < cvt.len; i += NYAN_BUFFER_SIZE)
    {
        int l = sendto(socketServer, &data[i], std::min(NYAN_BUFFER_SIZE, cvt.len - i), 0, (sockaddr*)&sourceAddress, sizeof(sockaddr_in));
        if(l <= 0)
            perror("sendto");

        SDL_Delay((Uint32)delay);
    }

    SDL_FreeWAV(data);
    free(cvt.buf);

    close(socketServer);
    
    freeaddrinfo(result);
}

bool Server::recvNegociation()
{
    uint8_t buffer[NYAN_MESSAGE_SIZE];
    unsigned int sourceAddressLen = sizeof(sourceAddress);

    // wait for a message
    ssize_t bufferLen = 0;
    while(bufferLen < NYAN_MESSAGE_SIZE)
    {
        bufferLen += recvfrom(socketServer, &buffer[bufferLen], sizeof(buffer) - bufferLen, 0, &sourceAddress, &sourceAddressLen);
    }
    // if asked for streaming
    if(std::string((const char*)buffer) == "Bonjour, je veux un stream.")
    {
        // reply the stream caracterics
        std::stringstream ss;
        ss << "Je vais streamer du son WAVE mono, de format " << asNyan.format << " à " << asNyan.freq << " Hz.";
        std::string str = ss.str();
        memcpy(buffer, str.c_str(), str.length() + 1);

        if(sendto(socketServer, buffer, sizeof(buffer), 0, &sourceAddress, sourceAddressLen) == -1)
        {
            perror("sendto");
            return false;
        }
    }

    // wait for ack
    bufferLen = 0;
    while(bufferLen < NYAN_MESSAGE_SIZE)
    {
        bufferLen += recvfrom(socketServer, &buffer[bufferLen], sizeof(buffer) - bufferLen, 0, &sourceAddress, &sourceAddressLen);
    }
    if(std::string((const char*)buffer) == "Ok j'attends ça avec impatience.")
    {
        return true;
    }

    return false;
}

}
