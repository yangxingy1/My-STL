#include "Unordered_map.h"

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::Unordered_map(size_type bucket_count)
: _size(0), max_load_factor(1.0)
{
    // 防止rehash 的*2操作出现问题
    if(bucket_count == 0)
        bucket_count = 8;
    buckets.reserve(bucket_count);
    for(size_type i=0; i<bucket_count; ++i)
        buckets.push_back(nullptr);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::Unordered_map(const Unordered_map& other)
: _size(0), max_load_factor(other.max_load_factor)
{
    size_type bucket_count = other.buckets.size();
    bucket_count = bucket_count != 0 ? bucket_count : 8;

    buckets.reserve(bucket_count);
    for(int i=0; i<bucket_count; ++i)
        buckets.push_back(nullptr);
    
    for(size_type i=0; i<other.buckets.size(); ++i)
    {
        node_type* curr = other.buckets[i];

        while(curr != nullptr)
        {
            insert(curr->data.first, curr->data.second);
            curr = curr->next;
        }
    }
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>& 
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::operator=(const Unordered_map& other)
{
    if(this == &other)
        return *this;
    
    clear();

    size_type bucket_count = other.buckets.size();
    
    Vector<node_type*> new_buckets;
    new_buckets.reserve(bucket_count);
    for(size_type i = 0; i < bucket_count; ++i) 
        new_buckets.push_back(nullptr);

    buckets = std::move(new_buckets);
    max_load_factor = other.max_load_factor;

    for(size_type i=0; i<other.buckets.size(); ++i)
    {
        node_type* curr = other.buckets[i];

        while(curr != nullptr)
        {
            insert(curr->data.first, curr->data.second);
            curr = curr->next;
        }
    }
    return *this;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::Unordered_map(Unordered_map&& other) noexcept
: _size(other._size), max_load_factor(other.max_load_factor), buckets(std::move(other.buckets))
{
    other._size = 0;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>&
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::operator=(Unordered_map&& other) noexcept
{
    if(this == &other)
        return *this;
    
    clear();

    buckets = std::move(other.buckets);
    max_load_factor = other.max_load_factor;
    _size = other._size;
    other._size = 0;

    return *this;
}


template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::~Unordered_map()
{
    clear();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
void Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::clear()
{
    for(size_type i=0; i<buckets.size(); ++i)
    {
        node_type* curr = buckets[i];
        while(curr != nullptr)
        {
            node_type* next = curr->next;
            // 调用Allocator析构+释放
            allocator.destroy(curr);
            allocator.deallocate(curr, 1);
            curr = next;
        }
        buckets[i] = nullptr;
    }
    _size = 0;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
typename Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::iterator 
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::find(const Key& key) 
{
    if(buckets.empty())
        return end();
    
    // 计算桶索引
    size_type index = buckets_index(key, buckets.size());

    node_type* curr = buckets[index];
    while(curr != nullptr)
    {
        if(key_equal(curr->data.first, key))
            return iterator(curr, this, index);
        curr = curr->next;
    }
    return end();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
std::pair<typename Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::node_type*, bool> 
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::insert(const Key& key, const Value& value)
{
    // 查重
    iterator tmp = find(key);
    if(tmp != end())
        return {tmp.current_node, false};

    // 阈值检测
    if(_size + 1 > buckets.size() * max_load_factor)
        rehash(buckets.size() * 2);
    
    // 构造结点
    node_type* temp = allocator.allocate(1);
    allocator.construct(temp, key, value);

    // 头插法插入
    size_type index = buckets_index(key, buckets.size());
    temp->next = buckets[index];
    buckets[index] = temp;

    ++_size;
    return {temp, true};
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
std::pair<typename Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::node_type*, bool> 
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::insert(const Key& key, Value&& value)
{
    // 查重
    iterator tmp = find(key);
    if(tmp != end())
        return {tmp.current_node, false};

    // 阈值检测
    if(_size + 1 > buckets.size() * max_load_factor)
        rehash(buckets.size() * 2);
    
    // 构造结点
    node_type* temp = allocator.allocate(1);
    allocator.construct(temp, key, std::move(value));

    // 头插法插入
    size_type index = buckets_index(key, buckets.size());
    temp->next = buckets[index];
    buckets[index] = temp;

    ++_size;
    return {temp, true};
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
void Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::rehash(size_type new_bucket_count)
{
    if(buckets.size() >= new_bucket_count)
        return;
    
    Vector<node_type*> new_buckets(new_bucket_count, nullptr);

    for(size_type i=0; i<buckets.size(); ++i)
    {
        node_type* curr = buckets[i];
        while(curr != nullptr)
        {
            node_type* nex = curr->next;
            // 计算新下标
            size_type new_index = buckets_index(curr->data.first, new_bucket_count);

            // 头插入新桶
            curr->next = new_buckets[new_index];
            new_buckets[new_index] = curr;

            curr = nex;
        }
        buckets[i] = nullptr;
    }

    buckets = std::move(new_buckets);
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Value& Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::operator[](const Key& key)
{
    iterator temp = find(key);
    if(temp != end())
        return temp->second;
    auto result = insert(key, Value());
    return result.first->data.second;
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
bool Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::erase(const Key& key)
{
    if(empty())
        return false;
    
    size_type index = buckets_index(key, buckets.size());
    node_type* curr = buckets[index];
    node_type* prev = nullptr;

    while(curr != nullptr)
    {
        if(key_equal(curr->data.first, key))
        {
            if(prev == nullptr)
                buckets[index] = curr->next;
            else
                prev->next = curr->next;

                allocator.destroy(curr);
            allocator.deallocate(curr, 1);
            --_size;
            return true;    
        }
        
        prev = curr;
        curr = curr->next;
    }
    return false;
}

