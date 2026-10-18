#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// class Student
// {
// private:
//     int *age;
//     int rank;

// public:
//     void Get_age(void)
//     {
//         cout << "年龄：" << *age << endl;
//         return;
//     }
//     Student(int val) // 构造函数：拿类名构造的函数
//     {
//         age = new int(val); // new申请新内存，类型* 指针 = new 类型(初始值);
//     }
//     ~Student() // 析构函数，也就是释放申请的内存
//     {
//         if (age != 0)
//         {
//             delete age;
//             age = nullptr;
//             cout << "已经销毁申请内存" << endl;
//         }
//     }
// };

int main()
{
    vector<int> number;

    number.push_back(59);
    number.push_back(45);
    number.push_back(76);
    number.push_back(106);

    sort(number.begin(), number.end());

    for (int i; i < number.size(); i++)
    {
        cout << number[i] << endl;
    }

    return 0;
}
