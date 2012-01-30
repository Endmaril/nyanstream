#include "circularbuffer.hh"

namespace nyanstream
{

CircularBuffer::CircularBuffer(int size)
    : size(size)
{
    data = new uint8_t[size];
}

CircularBuffer::~CircularBuffer()
{
    delete[] data;
}

}
