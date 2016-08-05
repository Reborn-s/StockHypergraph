#include "stdafx.h"
#include "StockHgraph.h"
#include<iostream>

using namespace std;


int main()
{
	
	StockHgraph* p=new StockHgraph();
	p->readData();
	p->createHgr("../output/ordinary.hgr");
	
}
