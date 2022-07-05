/* book.cpp */
#include "Book.h"  // ������Ҫ����BookImpl��ĳ�Ա������
                      // ����Ҫ����BookImpl�Ķ���ͷ�ļ�
#include "BookImpl.h"  // ��������ʵ��Book�࣬����Ҫ����Book��
                     // ��ͷ�ļ�

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

/* BookImpl���ʵ�ֺ��� */

void Book::BookImpl::print()
{
    std::cout << "print from BookImpl" << std::endl;
}
