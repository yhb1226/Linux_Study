#include <iostream>

using namespace std;

void swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}


int main(void)
{

    int x = 1, y = 2;
    swap(x,y);
    cout << "x=" << x << "y=" << y <<endl;

    return 0;
}
