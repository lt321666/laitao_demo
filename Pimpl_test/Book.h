
#ifndef PUBLIC_H_INCLUDED
#define PUBLIC_H_INCLUDED
class Book
{
public:
	Book();
	~Book();
	void print();

private:
	class BookImpl;  // Bookʵ�����ǰ������
	BookImpl* pimpl;
};


#endif