#include "client.hh"
#include "circularbuffer.hh"

namespace nyanstream
{

//unsigned int sound_cursor_read = 0;
//unsigned int sound_cursor_write = 0;
//Uint8* sound_buffer;
CircularBuffer<Uint16> buffer(NYAN_BUFFER_SIZE);

void audio_callback(void* userdata, Uint8* stream, int len)
{
    Uint16 sample = 0;
    Uint16* p = (Uint16*)stream;
    int i = 0;
    while (!buffer.isEmpty() && i < len )
    {
        buffer >> sample;
        p[i++] = sample;
        i++;
    }
    
    if (i < len)
    {
        if (buffer.getSize() < len/2)
            std::cerr << "Buffer undersized" << std::endl;
        else
            std::cerr << "Buffer underrun" << std::endl;
    }
    
    while (i < len)
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

    SDL_AudioSpec desired, obtained;
    desired.freq = 44100;
    desired.format = AUDIO_S16LSB;
    desired.channels = 1; // mono
    desired.samples = NYAN_BUFFER_SIZE;
    desired.callback = audio_callback;
    desired.userdata = NULL;
    if(SDL_OpenAudio(&desired, &obtained) < 0)
    {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(1);
    }
    std::cout << obtained.freq << std::endl;
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
        
        std::cout << len << ", free:" << freeSize << std::endl;
        buffer.print();
        if (!overflow)
            buffer.write((Uint16*)msg, freeSize);
        else
            std::cerr << "Buffer overflow" << std::endl;
        buffer.print();
        //memcpy(&sound_buffer[sound_cursor_write], msg, len);
        //sound_cursor_write += len;
    }

    close(sock1);
    
}

}
