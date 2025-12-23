

template<typename T>
Vector<T>::Vector(const int& size)
    : size(size), capacity(size)
{
    values = new T[capacity];
}

template<typename T>
Vector<T>::Vector(const int& size, const Value& value)
    : Vector(size)
{
    for(int i=0;i<size;++i)
        values[i] = value;
}

