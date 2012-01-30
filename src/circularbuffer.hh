#ifndef __NYAN_CIRCULARBUFFER__
#define __NYAN_CIRCULARBUFFER__

#include "common.hh"
#include <ostream>

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
        size_t newPosition = (positionWrite + 1) % size;
        if(newPosition != positionRead)
        {
            positionWrite = newPosition;
            data[positionWrite] = v;
        }

        return *this;
    }

    CircularBuffer& operator>>(T& v)
    {
        if(positionRead != positionWrite)
        {
            positionRead = (positionRead + 1) % size;
            v = data[positionRead];
        }

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

    void print()
    {
        std::cout << "size:" << size << ", r:" << positionRead << ", w:" << positionWrite << std::endl;
    }
};

}

#endif
