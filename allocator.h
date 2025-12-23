#ifndef YXY__STL__ALLOCATOR
#define YXY__STL__ALLOCATOR

#include<new>
#include<cstddef>
#include<utility>

// 内存分配类
template<typename T>
class Allocator
{
public:
    // --- STL 契约的类型定义 ---
    using value_type      = T;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;    

    template<class U>
    struct rebind
    {
        using other = Allocator<U>;
    }

public:
    // --- 构造及析构 ---
    Allocator() = default;
    Allocator(const Allocator&) = default;
    template<class U>
    Allocator(const Allocator<class U>&) noexcept {}
    ~Allocator() = default;

    pointer allocate(size_type n)
    {
        if(n == 0)
            return nullptr;
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }
    void deallocate(pointer p, size_type)
    {
        if(p == nullptr)
            return;
        ::operator delete(p);
    }
    void destroy();

}

#endif