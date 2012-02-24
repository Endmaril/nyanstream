#include <sstream>
#include <cassert>
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
    SDL_AudioCVT cvt;
    Uint8* data;

    bool recvNegociation();
public:
    Server(char* argv[]);
    ~Server();

    template<typename T>
    void serverLoop()
    {
        T* samples = (T*)data;
        int sampleSize = sizeof(T);
        size_t nSamples = cvt.len / sampleSize;
        float sendDelay = 1.0f / ((float)sampleSize * (float)asNyan.freq / (float)NYAN_MESSAGE_DATA_SIZE) * 0.5;
        size_t nSamplePerMessage = NYAN_MESSAGE_DATA_SIZE / sampleSize;

        for(size_t i = 0; i < nSamples; i += nSamplePerMessage)
        {
            size_t cnt1 = i / (44100 * 3),
                   cnt2 = i % (44100 * 3);
            // prepare message
            char message[NYAN_MESSAGE_SIZE];
            memset(message, 0, sizeof(message));
            size_t nSamplesMessage = std::min(nSamplePerMessage, i - nSamples);

            // header
            std::stringstream ss;
            ss << "NYAN " << cnt1 << " " << cnt2 << " " << nSamplesMessage;
            std::string strHeader(ss.str());
            assert(strHeader.length() < NYAN_MESSAGE_HEADER_SIZE);
            memcpy(message, strHeader.c_str(), strHeader.length() + 1);

            // data
            memcpy(&message[NYAN_MESSAGE_HEADER_SIZE], &samples[i], nSamplesMessage * sampleSize);

            // send message on network
            int l = sendto(socketServer, &message, NYAN_MESSAGE_SIZE, 0, (sockaddr*)&sourceAddress, sizeof(sockaddr_in));
            if(l <= 0)
                perror("sendto");

            SDL_Delay((Uint32)(sendDelay * 1000.0f));
        }

        /*
        for(int i = 0; i < cvt.len; i += NYAN_BUFFER_SIZE)
        {
            int l = sendto(socketServer, &data[i], std::min(NYAN_BUFFER_SIZE, cvt.len - i), 0, (sockaddr*)&sourceAddress, sizeof(sockaddr_in));
            if(l <= 0)
                perror("sendto");

            SDL_Delay((Uint32)(delay * 1000.0f));
        }
        */
    }

    int run();
};

}
