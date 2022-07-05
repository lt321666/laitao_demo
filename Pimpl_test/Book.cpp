/* book.cpp */
#include "Book.h"  // 我们需要调用BookImpl类的成员函数，
                      // 所以要包含BookImpl的定义头文件
#include "BookImpl.h"  // 我们正在实现Book类，所以要包含Book类
                     // 的头文件

Book::Book()
{
    pimpl = new BookImpl;
}

Book::~Book()
{
    delete pimpl;
}

void Book::print()
{
    pimpl->print();
}

/* BookImpl类的实现函数 */

void Book::BookImpl::print()
{
    std::cout << "print from BookImpl" << std::endl;
}
