
template<class T>
class Packet
{
    private:
        T* data;
        std::pair<size_t, size_t> position;
        size_t nData;
    
    public:
        Packet(T* data, std::pair<size_t,size_t> position, size_t nData) : 
            data(data),
            position(position),
            nData(nData)
        {
        }
        
        std::pair<size_t, size_t> getPosition()
        {
            return position;
        }
        
        T* getData()
        {
            return data;
        }
        
        size_t getNData()
        {
            return nData;
        }
};
