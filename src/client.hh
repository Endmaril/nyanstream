#include "common.hh"
#include "circularbuffer.hh"

namespace nyanstream
{
    class Client 
    {
        private:
            int                     sock1;
            CircularBuffer<Uint16>  buffer;
            SDL_AudioSpec           asDesired, asObtained;
            char**                   argv;
            
            static void audio_callback(void* userdata, Uint8* stream, int len);
            void audio_callback(Uint8* stream, int len);
            int negociate();
        
        public:
            Client(char* argv[]);
            int run();
    };

}
