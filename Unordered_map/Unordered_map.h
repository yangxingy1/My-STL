#ifndef YXY__STL__UNORDERED_MAP_H
#define YXY__STL__UNORDERED_MAP_H


#include <functional>    // std::hash
#include <utility>       // std::pair
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

    HashNode(const Key& key, const Value& value)
    : data(key, value), next(nullptr) {}

    HashNode(const Key& key, Value&& value)
    : data(key, std::move(value)), next(nullptr) {}

    ~HashNode() = default;
};

// 前向声明
template<
    typename Key,
    typename Value,
    typename Hash = std::hash<Key>,
    typename KeyEqual = std::equal_to<Key>,
    typename Alloc = Allocator<HashNode<Key, Value>>
>
class Unordered_map;

// 迭代器
template<typename Key, typename Value>
struct iterator
{
    using node_type          = HashNode;
    using value_type         = std::pair<Key, Value>;
    using reference          = value_type&;
    using pointer            = value_type*;
    using size_type          = size_t;

    // 当前指向的结点
    node_type* current_node;
    // 所属hashmap
    Unordered_map* map_instance;
    // 所在桶
    size_type current_bucket;

    // 构造
    iterator(node_type* node, Unordered_map* instance, size_type bucket)
    : current_node(node), map_instance(instance), current_bucket(bucket) {}

    // 重载 解引用 比较 自增
    reference operator*() const { return current_node -> data; }
    reference operator->() const { return &(current->data); }

    bool operator==(const iterator& other) const { return current_node == other.current_node; }
    bool operator!=(const iterator& other) const { return current_node != other.current_node; }

    iterator& operator++() 
    {
        // 当前链表未走完
        if(current_node->next != nullptr)
            current_node = current_node->next;
        // 后面的桶中找下一个非空点
        else
        {
            ++current_bucket;
            while(current_bucket < map_instance->buckets.size() && map_instance->buckets[current_bucket] == nullptr)
                ++current_bucket;
            
            // 找到非空结点
            if(current_bucket < map_instance->buckets.size())
                current_node = map_instance->buckets[current_bucket];
            // 到end
            else
                current_node = nullptr;
        }
        return *this;
    }


};

template<
    typename Key,
    typename Value,
    typename Hash,
    typename KeyEqual,
    typename Alloc 
>
class Unordered_map
{
public:
    using node_type            = HashNode<Key, Value>;
    using value_type           = std::pair<Key, Value>;
    using reference            = value_type&;
    using const_reference      = const value_type&;
    using size_type            = size_t;

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

public:
    // -------------------------------- 构造与析构 ----------------------------
    Unordered_map(size_type bucket_count = 8);
    // 拷贝
    Unordered_map(const Unordered_map& other);
    // 移动
    Unordered_map(Unordered_map&& other);


    ~Unordered_map();
    
    // -------------------------------- 常用方法 ------------------------------
    size_type size() { return _size; }
    bool empty() { return _size == 0 ;}

    void rehash(size_type new_bucket_count);

    // 插入
    std::pair<node_type*, bool> insert(const Key& key, const Value& value);
    std::pair<node_type*, bool> insert(const Key& key, Value&& value);

    // 删除
    bool erase();
    void clear();

    // 访问
    node_type* find(const Key& key);
    Value& operator[](const Key& key);

    // 友元声明 允许迭代器访问
    friend struct iterator<Key, Value>;
};

#include "Unordered_map.cpp"

#endif // YXY__STL__UNORDERED_MAP_H