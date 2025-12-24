#ifndef YXY__STL__VECTOR_H
#define YXY__STL__VECTOR_H

#include "../allocator.h"
#include<memory>

template<typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    using value_type        = T;
    using pointer           = T*;
    using iterator          = T*;
    using const_iterator    = const T*;
    using reference         = T&;
    using size_type         = std::size_t;

private:
    iterator start;
    iterator finish;                    // 最后一个数据后的地址
    iterator end_of_storage;            // 最后一个内存地址

    Alloc allocator;

public:
    // 构造函数
    Vector()
    : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}

    Vector(size_type capacity);

    Vector(size_type capacity, const value_type& value); 

    Vector(const Vector& other)
    {
        this->start = this->allocator.allocate(other.capacity);
        this->finish = this->start + other.size();
        this->end_of_storage = this->start + other.capacity;
    }

    //方法;
    size_type size() const
    {return finish - start}

    size_type capacity() const
    { return end_of_storage - start }

    bool empty() const
    { return start == finish}

    iterator begin() const
    { return start }

    iterator end() const
    { return end_of_storage }

    


};



#endif // YXY__STL__VECTOR_H