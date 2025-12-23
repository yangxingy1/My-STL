template <typename T>
class Vector
{
    using T = Value;
    using T* = atom;


private:
    int size;
    int capacity;
    T* values;
    // 开始/结束位置指针
    atom begin;
    atom end;

public:

    Vector(const int& size);
    Vector(const int& size, const Value& value);

}