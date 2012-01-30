#ifndef __NYAN_CIRCULARBUFFER__
#define __NYAN_CIRCULARBUFFER__

#include "common.hh"

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
        resize(size);
    }

    ~CircularBuffer()
    {
        delete[] data;
    }

    void resize(size_t newSize)
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
    
    bool isEmpty()
    {
        return positionRead == positionWrite;
    }
};

}

#endif
