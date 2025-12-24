#include<iostream>
#include<vector>

using namespace std;

int main()
{
    vector<int> a(10);
    cout << "size: " << a.size() << endl;
    cout << "capacity: " << a.capacity() << endl;
    return 0;
}