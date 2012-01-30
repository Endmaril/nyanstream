#ifndef __NYAN_CIRCULARBUFFER__
#define __NYAN_CIRCULARBUFFER__

#include "common.h"

namespace nyanstream
{

template<class T>
class CircularBuffer
{
private:
    size_t size;
    T* data;
    size_t positionWrite, positionRead;

public:
    CircularBuffer(size_t size)
    {
        resize(newSize);
    }

    ~CircularBuffer()
    {
        delete[] data;
    }

    void resize(size_t size)
    {
        size = newSize;
        data = new uint8_t[size];
        positionWrite = positionRead = 0;
    }

    CircularBuffer& operator<<(T v)
    {
        data[positionWrite] = v;
        positionWrite = (positionWrite + 1) % size;

        return *this;
    }

    CircularBuffer& operator>>(T& v)
    {
        v = data[positionRead];
        positionRead = (positionWrite + 1) % size;

        return *this;
    }
};

}

#endif
