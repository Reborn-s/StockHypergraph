#include<vector>
#include <direct.h>
#include <io.h>
#include <map>

#include"Stock.h"
using namespace std;

#define step 22

class StockHgraph
{
public:
	string			dataPath;
	vector<Stock*>		stocks;
	map<string,Stock*>	mapForFind;
	map<string,int>		mapForHgrNode;
	int				nodeCnt;
	string			nodeMap;
	int				nhedge;


public:
	StockHgraph();
	~StockHgraph();

	void readData();
	void createHgr(string	outPath);
	void writeEdge(int wgt, vector<string> nodes,fstream &hgr );
	void createHgrByStep(vector<vector<DayInfo>> comDayInfos, vector<vector<string>>	edgeSets,fstream &hgr,int cnt);
};