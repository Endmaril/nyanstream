#include "client.hh"
#include "circularbuffer.hh"

namespace nyanstream
{
    
Client::Client(char* argv[])
    : buffer(44100 * 3, 0), sock1(0), argv(argv)
{
}

void Client::audio_callback(void* userdata, Uint8* stream, int len)
{
    ((Client*) userdata) -> audio_callback(stream, len);
}

void Client::audio_callback(Uint8* stream, int len)
{
    Uint16 sample = 0;
    Uint16* p = (Uint16*)stream;
    int i = 0;
    while (i < len / 2 )
    {
        buffer >> sample;
        p[i++] = sample;
        //std::cout << sample << std::endl;
    }
}

int Client::negociate() 
{
    struct addrinfo *result;
    
    int error = getaddrinfo(argv[2], NULL, NULL, &result);
    if(error != 0)
    {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in si_serveur;
    memcpy(&si_serveur, result -> ai_addr, sizeof(si_serveur));
    int port = strtol(argv[3], (char**)NULL, 10);
    si_serveur.sin_port = htons(port);
    
    const char* data = "Bonjour, je veux un stream.";
    char buff[NYAN_MESSAGE_SIZE];
    strcpy(buff, data);
    sendto(sock1, buff, sizeof(buff), 0, (sockaddr*)&si_serveur, result->ai_addrlen);
    
    struct sockaddr exp;
    unsigned int lgdest = sizeof(sockaddr);
    
    int recv = 0;
    
    while (recv < sizeof(buff))
    {    
        recv += recvfrom(
            sock1,                                      /* descripteur de la socket de reception */
            buff + recv,                                /* adresse de recuperation du message    */
            sizeof(buff) - recv,                        /* taille de l'espace alloue a msg       */
            0,                                          /* toujours 0 pour le type SOCK_DGRAM    */
            &exp,                                       /* pour recuperer l'adr de l'expediteur  */
            &lgdest                                     /* taille espace reserve a dest          */
        );
    }
    
    std::string msg(buff);
    std::stringstream ss(msg);
    std::string dummy;
    int format, freq;
    //ss << "Je vais streamer du son WAVE mono, de format" << obtained.format << " à " << obtained.freq << " Hz.
    ss >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> format >> dummy >> freq;
    
    this -> format = format;
    asDesired.freq = freq;
    asDesired.format = format;
    asDesired.channels = 1; // mono
    asDesired.samples = NYAN_BUFFER_SIZE;
    asDesired.callback = audio_callback;
    asDesired.userdata = (void*) this;
    if(SDL_OpenAudio(&asDesired, &asObtained) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(1);
    }
    std::cout << asObtained.freq << std::endl;
    
    data = "Ok j'attends ça avec impatience.";
    strcpy(buff, data);
    sendto(sock1, buff, sizeof(buff), 0, (sockaddr*)&si_serveur, result->ai_addrlen);
}

int Client::run()
{
    sock1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock1 == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in si_listeningPort;

    memset((char *) &si_listeningPort, 0, sizeof(si_listeningPort));
    si_listeningPort.sin_family = AF_INET;
    int port = strtol(argv[3], (char **) NULL, 10);
    si_listeningPort.sin_port = htons(0);
    si_listeningPort.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock1, (sockaddr*)&si_listeningPort, sizeof(si_listeningPort))==-1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }

    struct sockaddr exp;
    unsigned int lgdest = sizeof(sockaddr);

    std::cout << "negociation begin" << std::endl;
    negociate();
    std::cout << "negociation end" << std::endl;

    //TODO: BuildCVT

    //sound_buffer = new Uint8[128 * 1024 * 1024];
    //sound_cursor_write = 0;
    //sound_cursor_read = 0;
    SDL_PauseAudio(0);
    
    switch(this->format)
    {
    case AUDIO_U8:
    case AUDIO_S8:
        std::cout << "8 bit sound" << std::endl;
        receiveLoop<Uint8>(sock1);
        break;
    case AUDIO_U16LSB:
    case AUDIO_S16LSB:
    case AUDIO_U16MSB:
    case AUDIO_S16MSB:
        std::cout << "16 bit sound" << std::endl;
        receiveLoop<Uint16>(sock1);
        break;
    }

    close(sock1);

}

}
