#include "Unordered_map.h"

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::Unordered_map(size_type bucket_count = 8)
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
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::~Unordered_map()
{
    clear();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
void Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::clear()
{
    for(size_type i=0; i<buckets.size(); ++i)
    {
        pointer curr = buckets[i];
        while(curr != nullptr)
        {
            pointer next = curr->next;
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
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::iterator 
Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::find(const Key& key) const
{
    if(buckets.empty)
        return end();
    
    // 计算桶索引
    size_type index = buckets_index(key, buckets.size());

    pointer curr = buckets[index];
    while(curr != nullptr)
    {
        if(curr->first == key)
            return iterator(curr, this, index);
        curr = curr->next;
    }
    return end();
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
std::pair<typename Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::pointer, bool> 
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
    pointer temp = allocator.allocate(1);
    allocator.construct(temp, key, value);

    // 头插法插入
    size_type index = buckets_index(key, buckets.size());
    temp->next = buckets[index];
    buckets[index] = temp;

    ++_size;
    return {temp, true};
}

template<typename Key, typename Value, typename Hash, typename KeyEqual, typename Alloc>
std::pair<typename Unordered_map<Key, Value, Hash, KeyEqual, Alloc>::pointer, bool> 
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
    pointer temp = allocator.allocate(1);
    allocator.construct(temp, key, value);

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
    
    Vector<pointer> new_buckets(new_bucket_count, nullptr);

    for(size_type i=0; i<buckets.size(); ++i)
    {
        pointer curr = buckets[i];
        while(curr != nullptr)
        {
            pointer nex = curr->next;
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

