
#ifndef PUBLIC_H_INCLUDED
#define PUBLIC_H_INCLUDED
class Book
{
public:
	Book();
	~Book();
	void print();

private:
	class BookImpl;  // Book实现类的前置声明
	BookImpl* pimpl;
};


#endif