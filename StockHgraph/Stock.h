#include<string>
#include<map>

#include"DayInfo.h"
using namespace std;

class Stock
{
public:
	string				name;
	int					node;
	map< int,int >	dayInfos;

public:
	Stock();
	~Stock();

	void setNext(Stock* p);
	Stock* getNext();
};