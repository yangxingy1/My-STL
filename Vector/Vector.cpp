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
        for(auto it = other.begin();it!=other.end();++it)
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
typename Vector<T, Alloc>::Vector& Vector<T, Alloc>::operator=(const Vector& rhs) 
{
    if(this != &rhs)
    {
        if(start)
        {
            for(auto it = start;it!=finish;++it)
                allocator.destroy(it);
            allocator.deallocate(start, capacity());
        }

        size_type len = rhs.size();
        size_type cap = rhs.capacity();
        
        start = allocator.allocate(cap);
        finish = start;
        end_of_storage = start + cap;

        for(auto it = rhs.start;it != rhs.finish;++it)
        {
            allocator.construct(finish, *it);
            ++finish;
        }
    }

    return *this;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::Vector& Vector<T, Alloc>::operator=(Vector&& rhs)
{
    if(this != &rhs)
    {
        if(start)
        {
            for(auto it = start;it!=finish;++it)
                allocator.destroy(it);
            allocator.deallocate(start, capacity());
        }

        start = rhs.start;
        finish = rhs.finish;
        end_of_storage = rhs.end_of_storage;

        rhs.start = nullptr;
        rhs.finish = nullptr;
        rhs.end_of_storage = nullptr;
    }
    return *this;
}


template<typename T, typename Alloc>
void Vector<T, Alloc>::reserve(size_type n)
{
    // ---> 若 新容量 <= 原容量 直接退出
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

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(const value_type& value)
{
    // 扩容
    if(finish == end_of_storage)
        reserve(capacity() != 0 ? capacity() * 2 : 1);
    
    allocator.construct(finish, value);
    ++finish;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::push_back(value_type&& value)
{
    // 扩容
    if(finish == end_of_storage)
        reserve(capacity() != 0 ? capacity() * 2 : 1);

    allocator.construct(finish, std::move(value));
    ++finish;
}

template<typename T, typename Alloc>
void Vector<T, Alloc>::pop_back()
{
    if(empty())
        return;
    
    allocator.destroy(--finish);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator pos, const value_type& value)
{
    size_type n = pos - start;
    if(size() != capacity()) 
    {
        for(auto it = finish - 1;it>=pos;--it)
        {
            allocator.construct(it+1, std::move(*it));
            allocator.destroy(it);
        }
        ++finish;
        allocator.construct(start + n, value);
        return start + n;
    }
    else
    {
        // 准备挪动
        iterator new_start = allocator.allocate(capacity() != 0 ? capacity() * 2 : 1);
        iterator new_finish = new_start;
        iterator new_end_of_storage = new_start + (capacity() != 0 ? capacity() * 2 : 1);
        iterator new_pos = start + n;
        auto it = start;
        
        // 挪前一段
        while(it < pos)
        {
            allocator.construct(new_finish, std::move(*it));
            ++it;
            ++new_finish;
        }
        // 构造新元素
        allocator.construct(new_finish++, value);
        // 挪后一段
        while(it<finish)
        {
            allocator.construct(new_finish, std::move(*it));
            ++it;
            ++new_finish;
        } 

        // 销毁原Vector
        for(auto i = start;i<finish;++i)
            allocator.destroy(i);
        allocator.deallocate(start, capacity());

        // 挪指针
        start = new_start;
        finish = new_finish;
        end_of_storage = new_end_of_storage;
        return start + n;
    }
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::insert(const_iterator pos, value_type&& value)
{
    size_type n = pos - start;
    if(size() != capacity()) 
    {
        for(auto it = finish - 1;it>=pos;--it)
        {
            allocator.construct(it+1, std::move(*it));
            allocator.destroy(it);
        }
        ++finish;
        allocator.construct(start + n, std::move(value));
        return start + n;
    }
    else
    {
        // 准备挪动
        iterator new_start = allocator.allocate(capacity() != 0 ? capacity() * 2 : 1);
        iterator new_finish = new_start;
        iterator new_end_of_storage = new_start + (capacity() != 0 ? capacity() * 2 : 1);
        iterator new_pos = start + n;
        auto it = start;
        
        // 挪前一段
        while(it < pos)
        {
            allocator.construct(new_finish, std::move(*it));
            ++it;
            ++new_finish;
        }
        // 构造新元素
        allocator.construct(new_finish++, std::move(value));
        // 挪后一段
        while(it<finish)
        {
            allocator.construct(new_finish, std::move(*it));
            ++it;
            ++new_finish;
        } 

        // 销毁原Vector
        for(auto i = start;i<finish;++i)
            allocator.destroy(i);
        allocator.deallocate(start, capacity());

        // 挪指针
        start = new_start;
        finish = new_finish;
        end_of_storage = new_end_of_storage;
        return start + n;
    }
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::erase(const_iterator pos)
{
    if(!pos || pos >= finish)
        return start + (pos - start);

    size_type n = pos - start;
    for(auto it = start + n;it<finish - 1;++it)
        *it = std::move(*(it+1));
    allocator.destroy(--finish);
    return start + n;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::iterator Vector<T, Alloc>::erase(const_iterator first, const_iterator last)
{
    if(!first || !last || last <= first || last > finish || first < start)
        return start + (first - start);
    
    size_type n = last - first;
    size_type k = first - start;
    for(auto it = start + k;it+n<finish;++it)
        *it = std::move(*(it + n));
    iterator new_finish = finish - n;
    for(auto it = new_finish;it<finish;++it)
        allocator.destroy(it);
    finish = new_finish;
    return start + k;
}
// 这里可以使用auto - C14特性来推到返回值类型 
// 这样更方便编写代码 但不够清晰 这里选择一个函数使用此特性
// 选择了front() 函数
template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference Vector<T, Alloc>::operator[](size_type n)
{
    return *(start + n);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference Vector<T, Alloc>::operator[](size_type n) const
{
    return *(start + n);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference Vector<T, Alloc>::at(size_type n)
{
    if(n >= size())
        throw std::out_of_range("Vector:: index out of range!");
    
    return *(start + n);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference Vector<T, Alloc>::at(size_type n) const
{
    if(n >= size())
        throw std::out_of_range("Vector:: index out of range!");

    return *(start + n);
}

template<typename T, typename Alloc>
auto& Vector<T, Alloc>::front() 
{
    return *start;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference Vector<T, Alloc>::front() const
{
    return *start;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::reference Vector<T, Alloc>::back()
{
    return *(finish - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_reference Vector<T, Alloc>::back() const
{
    return *(finish - 1);
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::pointer Vector<T, Alloc>::data() noexcept
{
    return start;
}

template<typename T, typename Alloc>
typename Vector<T, Alloc>::const_pointer Vector<T, Alloc>::data() const noexcept
{
    return start;
}