#include "Vector.h"

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_type capacity)
{
    start = allocator.allocate(capacity);
    finish = start;
    end_of_storage = start + capacity;
    try
    {
        while(finish != end_of_storage)
            allocator.construct(finish);
    }catch(...)
    {
        for(auto& it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(finish, capacity);
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(size_type capacity, const value_type& value)
{
    start = allocator.allocate(capacity);
    finish = start;
    end_of_storage = start + capacity;
    try
    {
        while(finish != end_of_storage)
            allocator.construct(finish, value);
    }catch(...)
    {
        for(auto& it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(finish, capacity);
        throw;
    }
}

