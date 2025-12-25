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
        {
            allocator.construct(finish);
            finish++;
        }
    }catch(...)
    {
        for(auto it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(start, capacity);
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
        {
            allocator.construct(finish, value);
            ++finish;
        }
    }catch(...)
    {
        for(auto it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(start, capacity);
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(std::initializer_list<value_type> il)
{
    size_type n = il.size();
    start = allocator.allocate(n);
    finish = start;
    end_of_storage = start + n;
    try
    {
        for(const auto& it : il)
        {
            allocator.construct(finish, it);
            finish++;
        }
    }
    catch(...)
    {
        for(auto it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(start, n);
        throw;
    }
}   

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const Vector& other)
{
    this->start = this->allocator.allocate(other.size());
    this->end_of_storage = this->start + other.size();
    finish = start;
    try
    {
        for(const auto it = other.begin();it!=other.end();++it)
        {
            this->allocator.construct(finish, *it);
            ++finish;
        }
    }
    catch(...)
    {
        for(auto it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(start, other.size());
        throw;
    }
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(Vector&& other) noexcept
: start(other.start), finish(other.finish), end_of_storage(other.end_of_storage)
{
    other.start = nullptr;
    other.finish = nullptr;
    other.end_of_storage = nullptr;
}

template<typename T, typename Alloc>
Vector<T, Alloc>::Vector(const_iterator first, const_iterator last)
{
    size_type n = last >= first ? last - first : 0;

    start = allocator.allocate(n);
    finish = start;
    end_of_storage = start + n;

    try
    {
        while(first != last)
        {
            allocator.construct(finish, *first);
            ++finish;
            ++first;
        }
    }
    catch(...)
    {
        for(auto it = start;it!=finish;++it)
            allocator.destroy(it);
        allocator.deallocate(start, n);
        throw;
    }
    
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_type n)
{
    // ---> 若新容量 <= 原容量 直接退出
    if(n <= capacity())
        return;
    
    // ---> 定义新指针
    pointer new_start = allocator.allocate(n);
    pointer new_finish = new_start;
    pointer new_end_of_storage = new_start + n;

    // ---> 移动/拷贝构造元素 -> 新容器内存地址
    // 此处try-catch中 
    // 若构造函数noexcept则使用移动构造 此时不会进入catch;
    // 否则使用拷贝构造保证原容器析构/释放内存时不会出现问题
    try
    {
        for(auto it = start;it!=finish;++it)
        {
            allocator.construct(new_finish, std::move_if_noexcept(*it));
            ++new_finish;
        }
    }
    catch(...)
    {
        for(auto it = new_start;it!=new_finish;++it)
            allocator.destroy(it);
        allocator.deallocate(new_start, n);
        throw;
    }
    
    // ---> 原容器元素析构并释放内存
    for(auto it = start;it!=finish;++it)
        allocator.destroy(it);
    allocator.deallocate(start, capacity());        // 此时指针指向不变     capacity()仍为原容器容量

    // ---> 更新指针指向
    start = new_start;
    finish = new_finish;
    end_of_storage = new_end_of_storage;
}