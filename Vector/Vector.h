#ifndef YXY__STL__VECTOR_H
#define YXY__STL__VECTOR_H

#include "../allocator.h"
#include<memory>
#include<initializer_list>
#include<stdexcept>

template<typename T, typename Alloc = Allocator<T>>
class Vector
{
public:
    using value_type        = T;
    using pointer           = T*;
    using const_pointer     = const T*;
    using iterator          = T*;
    using const_iterator    = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using size_type         = std::size_t;

private:
    iterator start;
    iterator finish;                    // 最后一个数据后的地址
    iterator end_of_storage;            // 最后一个内存地址

    Alloc allocator;

public:
    // ---------------------- 构造函数 --------------------------
    Vector()
    : start(nullptr), finish(nullptr), end_of_storage(nullptr) {}
    // 容量
    Vector(size_type capacity);
    // 容量 + 初值
    Vector(size_type capacity, const value_type& value);
    // 参数列表
    Vector(std::initializer_list<value_type> il);
    // 拷贝
    Vector(const Vector& other);    
    // 移动
    Vector(Vector&& other) noexcept;
    // 范围构造             暂不支持其他STL转换为Vector 只支持Vector的范围构造
    Vector(const_iterator first, const_iterator last);

    // ------------------------- 常用方法 ------------------------
    size_type size() const
    {return finish - start; }

    size_type capacity() const
    { return end_of_storage - start; }

    bool empty() const
    { return start == finish; }

    iterator begin() const
    { return start; }

    iterator end() const
    { return finish; }

    // 扩容
    void reserve(size_type n);

    // 尾插 - 左值: 拷贝
    void push_back(const value_type& value);

    // 尾插 - 右值: 移动
    void push_back(value_type&& value);

    // 尾出 
    void pop_back();

    // 插入  拷贝/移动
    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value);

    // 删除 返回删除位置迭代器
    iterator erase(const_iterator pos);
    // 区间删除
    iterator erase(const_iterator first, const_iterator last);

    // --------------------------- 访问 --------------------------
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    reference at(size_type n);
    const_reference at(size_type n) const; 

    // 这里可以使用auto - C14特性来推到返回值类型 
    // 这样更方便编写代码 但不够清晰 这里选择一个函数使用此特性
    // 选择了front() 函数
    auto& front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    pointer data() noexcept;
    const_pointer data() const noexcept;
};

#include "Vector.cpp"

#endif // YXY__STL__VECTOR_H