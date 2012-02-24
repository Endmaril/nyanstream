#ifndef __NYAN_CIRCULARBUFFER__
#define __NYAN_CIRCULARBUFFER__

#include "common.hh"
#include "Packet.hh"
#include <ostream>

namespace nyanstream
{

template<class T>
class CircularBuffer
{
private:
    size_t size;
    T* data;
    T defaultValue;
    std::pair<size_t, size_t> positionRead;

public:
    CircularBuffer(size_t size, T defaultValue) : defaultValue(defaultValue)
    {
        resize(size);
    }

    ~CircularBuffer()
    {
        delete[] data;
    }

    void resize(size_t newSize)
    {
        //~ if (data != NULL)
            //~ delete[] data;
        size = newSize;
        data = new T[size];
        for (size_t i = 0; i < size; i++)
        {
            data[i] = defaultValue;
        }
        positionRead = std::pair<size_t, size_t>(-1,size * 80 / 100);
    }

    CircularBuffer& operator<<(Packet<T> packet)
    {
        std::pair<size_t, size_t> packetPosition = packet.getPosition();
        if (packetPosition.second >= size)
            return *this;
        
        if(packetPosition.first - positionRead.first == 0 && packetPosition.second >= positionRead.second)
        {
            T* packetData = packet.getData();
            data[packetPosition.second] = packetData[0];
            
            for (int i = packetPosition.second + 1, j = 1; 
                 i != positionRead.second && j < packet.getNData(); 
                 i = (i+1) % size, j++)
            {
                data[i] = packetData[j];
            }
        }
        
        if(packetPosition.first - positionRead.first == 1 && packetPosition.second < positionRead.second)
        {
            T* packetData = packet.getData();
            for (int i = packetPosition.second, j = 0; 
                 i != positionRead.second && j < packet.getNData(); 
                 i = (i+1) % size, j++)
            {
                data[i] = packetData[j];
            }
        }
        return *this;
    }

    CircularBuffer& operator>>(T& v)
    {
        v = data[positionRead.second];
        data[positionRead.second] = defaultValue;
        positionRead.second = (positionRead.second + 1);
        if (positionRead.second == size)
        {
            positionRead.first ++;
        }
        positionRead.second %= size;
        return *this;
    }
    
    std::pair<size_t, size_t> getReadPos()
    {
        return positionRead;
    }

    void read(T* d, size_t n)
    {
        for(unsigned int i = 0; i < n; i++)
            *this >> d[i];
    }

    void write(Packet<T>* d, size_t n)
    {
        for(unsigned int i = 0; i < n; i++)
            *this << d[i];
    }

    size_t getSize()
    {
        return size;
    }

    void print()
    {
        std::cout << "size:" << size << ", r:" << positionRead.second << std::endl;
    }
};

}

#endif
