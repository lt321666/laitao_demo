#include <iostream>
#include <functional>
#include <memory>

using namespace std;
using namespace std::placeholders;

void StaticBindFunc(int a, int b)
{
    std::cout << "静态函数被调用,a =" << a << ",b=" << b << std::endl;
}

int main()
{
    // 1 使用占位符先占位，然后执行时再传参
    auto staticFunc1 = std::bind(&StaticBindFunc, std::placeholders::_1, std::placeholders::_2);
    staticFunc1(1, 2);

    // 2 使用一个占位符先占位，然后执行时传一个参
    auto staticFunc2 = std::bind(&StaticBindFunc, std::placeholders::_1, 4);
    staticFunc2(3);
    auto staticFunc3 = std::bind(&StaticBindFunc, 5, std::placeholders::_1);
    staticFunc3(6);

    // 3 使用占位符先占位，但是参数位置调换
    auto staticFunc4 = std::bind(&StaticBindFunc, std::placeholders::_2, std::placeholders::_1);
    staticFunc4(7, 8);

    getchar();
    return 0;
}

