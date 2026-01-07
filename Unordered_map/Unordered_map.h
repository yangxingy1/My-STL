#ifndef YXY__STL__UNORDERED_MAP_H
#define YXY__STL__UNORDERED_MAP_H


#include<functional>    // std::hash
#include<utility>       // std::pair
#include "../allocator.h"
#include "../Vector/Vector.h"

/* 
--- 使用桶(指针数组) + 链表 
--- 链表结点中存储key + value 
--- key->hash作为数组下标
--- hash因子过大时rehash负载均衡解决
*/

// 哈希结点
template<typename Key, typename Value>
struct HashNode
{
    // pair存储数据
    std::pair<const Key, Value> data;

    HashNode* next;

    HashNode(const Key& key, Value& value)
    : data(key, value), next(nullptr) {}

    HashNode(const Key& key, Value&& value)
    : data(key, std::move(value)), next(nullptr) {}

    ~HashNode() = default;
};

template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename Alloc = Allocator<HashNode<Key, Value>>
>
class Unordered_map
{
public:
    using node_type = HashNode<Key, Value>;
    using size_type = size_t;

private:
    // 桶数组
    Vector<node_type*> buckets;
    // 元素总数
    size_type _size;
    
    // 辅助器
    Alloc allocator;
    KeyEqual key_equal;
    Hash hasher;

    // rehash阈值
    double max_load_factor = 1.00;

    // 计算key对应的下标
    size_type buckets_index(const Key& key, size_type bucket_count) const
    {
        return hasher(key) % bucket_count;
    }
};

#include "Unordered_map.cpp"

#endif // YXY__STL__UNORDERED_MAP_H