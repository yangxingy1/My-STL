#include "Vector/Vector.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector> 
#include <initializer_list>

// =========================================================
// 辅助工具
// =========================================================

// 自动检查 Vector 内容是否符合预期的辅助函数
template<typename T>
void check_vec(const Vector<T>& v, std::initializer_list<T> expected, const std::string& msg) {
    if (v.size() != expected.size()) {
        std::cerr << "FAIL: " << msg << " - Size mismatch! Expected " << expected.size() << ", got " << v.size() << std::endl;
        exit(1);
    }
    size_t i = 0;
    for (const auto& val : expected) {
        if (v[i] != val) {
            std::cerr << "FAIL: " << msg << " - Content mismatch at index " << i << ". Expected " << val << ", got " << v[i] << std::endl;
            exit(1);
        }
        i++;
    }
    std::cout << "PASS: " << msg << std::endl;
}

// =========================================================
// 1. 基础构造函数测试
// =========================================================
void test_constructors() {
    std::cout << "\n=== 1. Testing Constructors ===" << std::endl;

    Vector<int> v1;
    assert(v1.empty());
    std::cout << "PASS: Default Constructor" << std::endl;

    Vector<int> v2(5, 10);
    check_vec(v2, {10, 10, 10, 10, 10}, "Fill Constructor");

    Vector<int> v3 = {1, 2, 3, 4, 5};
    check_vec(v3, {1, 2, 3, 4, 5}, "Initializer List Constructor");

    Vector<int> v4(v3);
    check_vec(v4, {1, 2, 3, 4, 5}, "Copy Constructor");
    assert(v4.begin() != v3.begin()); // 确保是深拷贝

    Vector<int> v5(std::move(v4));
    check_vec(v5, {1, 2, 3, 4, 5}, "Move Constructor");
    assert(v4.size() == 0); // 确保源对象被掏空
}

// =========================================================
// 2. 插入测试 (重点)
// =========================================================
void test_insert() {
    std::cout << "\n=== 2. Testing Insert ===" << std::endl;

    Vector<int> v = {10, 20, 30};
    
    // A. 头部插入 (不扩容)
    v.reserve(10); // 预留空间，确保不扩容
    auto it = v.insert(v.begin(), 5);
    check_vec(v, {5, 10, 20, 30}, "Insert at begin (no realloc)");
    assert(*it == 5); // 检查返回值

    // B. 中间插入
    it = v.insert(v.begin() + 2, 15);
    check_vec(v, {5, 10, 15, 20, 30}, "Insert in middle");
    assert(*it == 15);

    // C. 尾部插入
    it = v.insert(v.end(), 35);
    check_vec(v, {5, 10, 15, 20, 30, 35}, "Insert at end");
    assert(*it == 35);

    // D. 触发扩容的插入
    Vector<int> v_realloc = {1, 2};
    // 假设当前 capacity 是 2，插入第三个会触发扩容
    // 你的策略是 capacity * 2
    auto old_ptr = v_realloc.data();
    v_realloc.insert(v_realloc.begin() + 1, 100);
    
    check_vec(v_realloc, {1, 100, 2}, "Insert triggering reallocation");
    assert(v_realloc.capacity() >= 3);
    assert(v_realloc.data() != old_ptr); // 确保换了内存地址
}

// =========================================================
// 3. 删除测试 (重点)
// =========================================================
void test_erase() {
    std::cout << "\n=== 3. Testing Erase ===" << std::endl;

    // --- 单个删除 ---
    Vector<int> v = {1, 2, 3, 4, 5};

    // A. 删除头部
    auto it = v.erase(v.begin());
    check_vec(v, {2, 3, 4, 5}, "Erase begin");
    assert(*it == 2); // 返回值应该指向下一个元素

    // B. 删除中间
    it = v.erase(v.begin() + 1); // 删除 3
    check_vec(v, {2, 4, 5}, "Erase middle");
    assert(*it == 4);

    // C. 删除尾部
    it = v.erase(v.end() - 1); // 删除 5
    check_vec(v, {2, 4}, "Erase end");
    assert(it == v.end()); // 返回值应该等于 end()

    // --- 区间删除 ---
    Vector<int> v2 = {10, 20, 30, 40, 50, 60};
    
    // D. 删除中间一段 [20, 30, 40] -> 删除 20, 30, 40
    // begin()+1 指向 20
    // begin()+4 指向 50 (不包含)
    it = v2.erase(v2.begin() + 1, v2.begin() + 4);
    check_vec(v2, {10, 50, 60}, "Erase range (middle)");
    assert(*it == 50);

    // E. 删除全部
    v2.erase(v2.begin(), v2.end());
    assert(v2.empty());
    assert(v2.size() == 0);
    std::cout << "PASS: Erase range (all)" << std::endl;
    
    // F. 删除空区间 (应该无事发生)
    v2 = {1, 2, 3};
    v2.erase(v2.begin(), v2.begin());
    check_vec(v2, {1, 2, 3}, "Erase empty range");
}

// =========================================================
// 4. 复杂对象测试 (测试 std::move 是否生效)
// =========================================================
void test_complex_type() {
    std::cout << "\n=== 4. Testing Complex Type (std::string) ===" << std::endl;
    
    Vector<std::string> vs;
    vs.push_back("A");
    vs.push_back("B");
    vs.push_back("C");

    // 测试插入右值 (应该调用 move insert)
    vs.insert(vs.begin() + 1, std::string("INSERTED"));
    // A, INSERTED, B, C
    
    assert(vs[1] == "INSERTED");
    
    // 测试 erase 时的对象搬运
    vs.erase(vs.begin()); 
    // INSERTED, B, C
    
    assert(vs[0] == "INSERTED");
    assert(vs[1] == "B");
    assert(vs.size() == 3);

    std::cout << "PASS: Complex Type Manipulation" << std::endl;
}

// =========================================================
// 5. 迭代器测试
// =========================================================
void test_iterators() {
    std::cout << "\n=== 5. Testing Iterators ===" << std::endl;
    Vector<int> v = {10, 20, 30, 40};
    
    int sum = 0;
    for(auto it = v.begin(); it != v.end(); ++it) {
        sum += *it;
    }
    assert(sum == 100);
    std::cout << "PASS: Iterator Loop" << std::endl;
}

int main() {
    try {
        test_constructors();
        test_insert();
        test_erase();
        test_complex_type();
        test_iterators();
        
        std::cout << "\n===============================" << std::endl;
        std::cout << " ALL TESTS PASSED SUCCESSFULLY " << std::endl;
        std::cout << "===============================" << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "\n!!! EXCEPTION CAUGHT: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "\n!!! UNKNOWN EXCEPTION CAUGHT" << std::endl;
        return 1;
    }
    return 0;
}