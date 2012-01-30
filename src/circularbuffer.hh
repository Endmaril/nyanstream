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
        size = newSize + 1;
        data = new T[size];
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

    void read(T* d, size_t n)
    {
        for(unsigned int i = 0; i < n; i++)
            *this >> d[i];
    }

    void write(T* d, size_t n)
    {
        for(unsigned int i = 0; i < n; i++)
            *this << d[i];
    }

    size_t getSize()
    {
        return size;
    }

    size_t getFreeSize()
    {
        if(positionWrite > positionRead)
        {
            return positionWrite - positionRead;
        }
        else
        {
            return positionWrite + size - positionRead;
        }
    }
};

}

#endif
