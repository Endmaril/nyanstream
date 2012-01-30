#ifndef __NYAN_CIRCULARBUFFER__
#define __NYAN_CIRCULARBUFFER__

#include "common.h"

namespace nyanstream
{

class CircularBuffer
{
private:
    int size;
    uint8_t* data;

public:
    CircularBuffer(int size);
    ~CircularBuffer();

    
};

}

#endif
