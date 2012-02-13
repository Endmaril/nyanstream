
template<class T>
class Packet
{
    private:
        T data;
        std::pair<size_t, size_t> position;
    
    public:
        Packet(T data, size_t nBuffPos, size_t posInBuff) : 
            data(data),
            nBuffPos(nBuffPos),
            posInBuff(posInBuff)
        {
        }
        
        std::pair<size_t, size_t> getPosition()
        {
            return position;
        }
        
        T getData()
        {
            return data;
        }
}
