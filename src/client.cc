#include "client.hh"
#include "circularbuffer.hh"
#include <sstream>

namespace nyanstream
{

<<<<<<< HEAD
//unsigned int sound_cursor_read = 0;
//unsigned int sound_cursor_write = 0;
//Uint8* sound_buffer;
CircularBuffer<Uint16> buffer(NYAN_BUFFER_SIZE);
=======
Client::Client(char* argv[])
    : buffer(44100 * 3), sock1(0), argv(argv)
{
}

void Client::audio_callback(void* userdata, Uint8* stream, int len)
{
    ((Client*) userdata) -> audio_callback(stream, len);
}
>>>>>>> e8471af0040ba9deeb08c90b38f5636dac5a998a

void Client::audio_callback(Uint8* stream, int len)
{
    Uint16 sample = 0;
    Uint16* p = (Uint16*)stream;
    int i = 0;
    while (!buffer.isEmpty() && i < len / 2 )
    {
        buffer >> sample;
        p[i++] = sample;
    }
    
    if (i < len / 2)
    {
        if (buffer.getSize() < len/2)
            std::cerr << "Buffer undersized" << std::endl;
        else
            std::cerr << "Buffer underrun" << std::endl;
    }
    
    while (i < len / 2)
    {
        p[i++] = 0;
    }
    
    //if(sound_cursor_read + len > sound_cursor_write)
    //{
        //Uint16* p = (Uint16*)stream;
        //for(unsigned int i = 0; i < len / 2; i++)
        //{
            //p[i] = 0;
        //}
    //}
    //else
    //{
        //memcpy(stream, &sound_buffer[sound_cursor_read], len);
        //sound_cursor_read += len;
    //}
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
    char buff[1337];
    strcpy(buff, data);
    sendto(sock1, buff, sizeof(buff), 0, (sockaddr*)&si_serveur, result->ai_addrlen);
    
    struct sockaddr exp;
    unsigned int lgdest = sizeof(sockaddr);
    
    int recv = 0;
    
    while (recv < sizeof(buff))
    {    
        recv += recvfrom(
            sock1,                                      /* descripteur de la socket de reception */
            buff,                                       /* adresse de recuperation du message    */
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

    negociate();
    
    //TODO: BuildCVT

    //sound_buffer = new Uint8[128 * 1024 * 1024];
    //sound_cursor_write = 0;
    //sound_cursor_read = 0;
    SDL_PauseAudio(0);
    
    char msg[sizeof(Uint16) * buffer.getSize()];
    
    while(1)
    {
        bool overflow = false;
        size_t freeSize = buffer.getFreeSize();
        
        if (freeSize == 0)
        {
            freeSize = NYAN_BUFFER_SIZE / 2;
            overflow = true;
        }
        
        int len = recvfrom(
            sock1,                                      /* descripteur de la socket de reception */
            msg,                                        /* adresse de recuperation du message    */
            freeSize * sizeof(Uint16),                  /* taille de l'espace alloue a msg       */
            0,                                          /* toujours 0 pour le type SOCK_DGRAM    */
            &exp,                                       /* pour recuperer l'adr de l'expediteur  */
            &lgdest                                     /* taille espace reserve a dest          */
        );
        
        SDL_LockAudio();
        std::cout << len / 2 << std::endl;
        buffer.print();
        if (!overflow)
            buffer.write((Uint16*)msg, len / sizeof(Uint16));
        else
            std::cerr << "Buffer overflow" << std::endl;
        buffer.print();
        SDL_UnlockAudio();
        //memcpy(&sound_buffer[sound_cursor_write], msg, len);
        //sound_cursor_write += len;
    }

    close(sock1);
    
}

}
