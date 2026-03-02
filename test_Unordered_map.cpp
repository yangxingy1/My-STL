#include <iostream>
#include <string>
#include <cassert>
#include "Unordered_map/Unordered_map.h" // 确保路径对应

using namespace std;

// 测试用的工具函数
void check(bool condition, const string& test_name) {
    if (condition) {
        cout << "[PASS] " << test_name << endl;
    } else {
        cout << "[FAIL] " << test_name << " 💥" << endl;
        exit(1);
    }
}

int main() {
    cout << "=== 🚀 开始测试 Unordered_map ===" << endl;

    // 1. 基础插入与查找
    Unordered_map<string, int> m1;
    check(m1.empty(), "初始状态应为空");
    check(m1.size() == 0, "初始大小为 0");

    auto res1 = m1.insert("apple", 10);
    check(res1.second == true, "插入新 Key 应该成功");
    check(m1.size() == 1, "插入后大小为 1");

    auto res2 = m1.insert("apple", 20); // 查重测试
    check(res2.second == false, "重复插入应该失败");
    check(m1.size() == 1, "重复插入大小不变");

    // 2. operator[] 测试
    m1["banana"] = 5; // 自动插入默认值 0，然后赋值为 5
    check(m1.size() == 2, "operator[] 插入新值");
    check(m1["banana"] == 5, "operator[] 修改值成功");
    check(m1["apple"] == 10, "operator[] 读取存在的值");

    // 3. 扩容与哈希冲突测试 (触发 rehash)
    // 默认 initial buckets 可能是 8，插入 15 个必定触发 rehash
    for (int i = 0; i < 15; ++i) {
        m1.insert("key" + to_string(i), i);
    }
    check(m1.size() == 17, "大批量插入引发扩容 (Rehash)");
    check(m1["key10"] == 10, "扩容后数据依然能查到");

    // 4. 迭代器遍历测试
    int count = 0;
    for (auto it = m1.begin(); it != m1.end(); ++it) {
        count++;
    }
    check(count == 17, "正向迭代器完整遍历所有节点");

    // 5. 删除测试 (Erase)
    bool e1 = m1.erase("banana");
    check(e1 == true, "删除存在的元素");
    check(m1.size() == 16, "删除后大小减 1");

    bool e2 = m1.erase("not_exist");
    check(e2 == false, "删除不存在的元素返回 false");

    // 6. 深拷贝测试 (拷贝构造)
    Unordered_map<string, int> m2(m1);
    check(m2.size() == 16, "拷贝构造大小一致");
    m2["apple"] = 999;
    check(m1["apple"] == 10, "深拷贝：修改 m2 不影响 m1");

    // 7. 深拷贝测试 (拷贝赋值)
    Unordered_map<string, int> m3;
    m3.insert("temp", 1);
    m3 = m1;
    check(m3.size() == 16, "拷贝赋值大小一致");
    check(m3.find("temp") == m3.end(), "拷贝赋值前被正确 clear");

    // 8. 移动语义测试
    Unordered_map<string, int> m4(std::move(m2));
    check(m4.size() == 16, "移动构造接管数据");
    check(m2.size() == 0, "被移动对象(m2)大小被置 0");
    check(m2.empty(), "被移动对象(m2)为空");

    Unordered_map<string, int> m5;
    m5 = std::move(m3);
    check(m5.size() == 16, "移动赋值接管数据");
    check(m3.size() == 0, "被移动赋值对象(m3)大小置 0");

    // 9. clear 与析构测试
    m5.clear();
    check(m5.size() == 0, "clear 后大小为 0");
    check(m5.empty(), "clear 后 empty 为 true");
    auto it = m5.begin();
    check(it == m5.end(), "clear 后 begin() == end()");

    cout << "=======================================" << endl;
    cout << "🎉 所有测试全部通过！你的 Unordered_map 毕业啦！" << endl;
    return 0;
}