#include<iostream>
#include<vector>

using namespace std;

int main()
{
    vector<int> a(10);
    a[0] = 1;
    a[1] = 2;
    vector<int> b(a);
    cout << "size: " << a.size() << endl;
    cout << "capacity: " << a.capacity() << endl;
    cout << "size: " << b.size() << endl;
    cout << "capacity: " << b.capacity() << endl;
    return 0;
}