#ifndef YXY__STL__ALLOCATOR_H
#define YXY__STL__ALLOCATOR_H

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

    // 分配/释放内存 构造
    pointer allocate(size_type n)
    {
        if(n == 0)
            return nullptr;
        // 只分配内存
        return static_cast<pointer>(::operator new(n * sizeof(value_type)));
    }
    void deallocate(pointer p, size_type)
    {
        if(p == nullptr)
            return;
        ::operator delete(p);
    }
    
    // 容器内构造/析构类 调用构造函数
    template<typename U, typename... Args>          // Args -> 构造类需要的所有参数
    void construct(U* p, Args&&... args)
    {
        // 不分配内存只构造
        ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }
    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }

    // 计算最多元素个数
    size_type max_size() const noexcept
    {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    // 比较运算符重载 泛型 -> 一致
    friend bool operator==(const Allocator&, const Allocator&) {return true};
    friend bool operator!=(const Allocator&, const Allocator&) {return false};


};

#endif // YXY__STL__ALLOCATOR_H