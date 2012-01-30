#include "server.hh"

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
    int port = strtol(argv[3], (char**)NULL, 10);
    si_client.sin_port = htons(port);

    // read sound
    SDL_AudioSpec wave;
    SDL_AudioCVT cvt;
    Uint8* data;
    Uint32 dlen;
    if(SDL_LoadWAV("data/nyan.wav", &wave, &data, &dlen) == NULL)
    {
        fprintf(stderr, "SDL_LoadWAV(%s): %s\n", "data/nyan.wav", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16, 2, 44100);
    cvt.buf = (Uint8*)malloc(dlen * cvt.len_mult);
    memcpy(cvt.buf, data, dlen);
    cvt.len = dlen;
    SDL_ConvertAudio(&cvt);

    std::cout << "Wav is " << dlen << " long" << std::endl;

    for(int i = 0; i < cvt.len; i += NYAN_BUFFER_SIZE)
    {
        int l;
        l = sendto(sock1, &data[i], std::min(NYAN_BUFFER_SIZE, cvt.len - i), 0, (sockaddr*)&si_client, result->ai_addrlen);
        if(l <= 0)
            perror("sendto");
        SDL_Delay(40);
    }

    SDL_FreeWAV(data);
    free(cvt.buf);

    close(sock1);
    
    freeaddrinfo(result);
}

}
