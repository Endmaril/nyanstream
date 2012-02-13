#include "common.hh"
#include "circularbuffer.hh"
#include <sstream>

namespace nyanstream
{
    class Client 
    {
        private:
            int                     sock1;
            CircularBuffer<Uint16>  buffer;
            SDL_AudioSpec           asDesired, asObtained;
            char**                  argv;
            int                     format;
            
            static void audio_callback(void* userdata, Uint8* stream, int len);
            void audio_callback(Uint8* stream, int len);
            int negociate();
        
        public:
            Client(char* argv[]);
            int run();
            
            template<class T>
            void receiveLoop(int sock1)
            {
                while(1)
                {
                    Packet<Uint16> packet = retreivePacket<Uint16>(sock1);
                    if (packet.getNData() == 0) {
                        continue;
                    }
                    SDL_LockAudio();
                    
                    buffer << packet;
                    
                    SDL_UnlockAudio();
                    //memcpy(&sound_buffer[sound_cursor_write], msg, len);
                    //sound_cursor_write += len;
                }
            }
            
            template<class T>
            Packet<T> retreivePacket(int sock1)
            {
                struct sockaddr exp;
                unsigned int lgdest = sizeof(sockaddr);
                size_t receivedSize = 0;
                char buffer[NYAN_MESSAGE_SIZE];
                
                while(receivedSize < NYAN_MESSAGE_SIZE)
                {
                    int len = recvfrom(
                        sock1,                                      /* descripteur de la socket de reception */
                        buffer + receivedSize,                      /* adresse de recuperation du message    */
                        NYAN_MESSAGE_SIZE - receivedSize,           /* taille de l'espace alloue a msg       */
                        0,                                          /* toujours 0 pour le type SOCK_DGRAM    */
                        &exp,                                       /* pour recuperer l'adr de l'expediteur  */
                        &lgdest                                     /* taille espace reserve a dest          */
                    );
                    receivedSize += len;
                }
                std::string msg(buffer);
                std::stringstream ss(msg);
                std::string dummy;
                size_t numBuff = 0, posInBuff = 0, nData = 0;
                
                ss >> dummy >> numBuff >> posInBuff >> nData;
                if (dummy != "NYAN")
                {
                    return Packet<T>(NULL, std::pair<size_t, size_t>(0,0), 0);
                }
                T data[nData * sizeof(T)];
                memcpy(data, &buffer[NYAN_MESSAGE_HEADER_SIZE], sizeof(data));
                return Packet<T>(data, std::pair<size_t, size_t>(numBuff,posInBuff), nData);
            }
    };

}
