#include<String>
#include<fstream>
#include<iostream>
#include<algorithm>
#include <Windows.h>

#include"StockHgraph.h"

using namespace std;

/*
DWORD start,end;
start = GetTickCount();
end = GetTickCount();
cout<<"time: "<<start-end<<endl;
*/


StockHgraph::StockHgraph()
{
	dataPath =	"../m2.txt";
	nodeMap	=	"";
	nodeCnt =	1;
	nhedge	=	0;
}

StockHgraph::~StockHgraph()
{
}

void StockHgraph::readData()
{
	fstream datafile;
	datafile.open(dataPath,ios::in);

	if(!datafile)
	{
		cout<<"error open!";
		exit(0);
	}

	char line[5000] = {0};
	int day=0;
	while(!datafile.eof())
	{
		datafile.getline(line,sizeof(line));
		string name = "";
		
		for(int i=0;line[i]!='\0';i++)
		{
			
			if(line[i]!=',')
				name+=line[i];
			if(line[i]==','||(line[i]!=','&&line[i+1]=='\0'))
			{
				map<string,Stock*>::iterator iter = mapForFind.find(name);
				if(iter==mapForFind.end())
				{
					Stock* stock = new Stock();
					stock->name=name;
					if(day%2==0)
						stock->dayInfos[day] = 1;
					else
						stock->dayInfos[day] = -1;
					mapForFind[name] = stock;
					stocks.push_back(stock);
				}
				else
				{
					if(day%2==0)
						mapForFind[name]->dayInfos[day] = 1;
					else
						mapForFind[name]->dayInfos[day] = -1;
				}
				name="";
			}
		}

		day++;
	}
}

void StockHgraph::writeEdge(int wgt,vector<string> nodes,fstream &hgr)
{
	hgr<<wgt<<" ";
	for(unsigned int i=0;i<nodes.size();i++)
	{
		char num[5];
		if(mapForHgrNode.find(nodes[i])==mapForHgrNode.end())
		{
			mapForHgrNode[nodes[i]] = nodeCnt;
			_itoa_s(nodeCnt,num,10);
			hgr<<num<<" ";
			nodeCnt++;
			nodeMap=nodeMap+num+":"+nodes[i]+"\n";
		}
		else
		{
			_itoa_s(mapForHgrNode[nodes[i]],num,10);
			hgr<<mapForHgrNode[nodes[i]]<<" ";
		}
	}
	hgr<<"\n";
	
}

bool sortVec(const string &v1,const string &v2)
{
	return v1<v2;
}


void StockHgraph::createHgrByStep(vector<vector<DayInfo>> oldcomDI, vector<vector<string>> edgeSets,fstream &hgr,int cnt)
{
	int ifreturn=0;
	for(unsigned int a=0;a<oldcomDI.size();a++)
	{
		for(unsigned int b=0;b<oldcomDI[a].size();b++)
		{
			if(oldcomDI[a][b].upOrdown==1||oldcomDI[a][b].upOrdown==0)
				ifreturn++;
		}
	}
	if(ifreturn==0)
		return;
	
	cout<<cnt<<" nodes then..."<<endl;
	cout<<"hedge number: "<<nhedge<<endl;

	vector<vector<DayInfo>> newcomDI;
	vector<vector<string>> newEdgeSets;

	for(unsigned int i=0;i<stocks.size();i++)
	{
		
		for(unsigned int j=0;j<edgeSets.size();j++)
		{
			int same = 0;

			vector<DayInfo> comDayInfos;
			vector<string>::iterator iter=find(edgeSets[j].begin(),edgeSets[j].end(),stocks[i]->name);
			if(iter==edgeSets[j].end())
			{
				vector<string> thisedge=edgeSets[j];
				vector<DayInfo> thisDInfo=oldcomDI[j];		//超边对应的比较矩阵
				for(map<int,int>::iterator iter=stocks[i]->dayInfos.begin();iter!=stocks[i]->dayInfos.end();iter++)
				{
					int day = iter->first;
					int i_UorD=iter->second;
					DayInfo* newDInfo = new DayInfo();
					newDInfo->day=day;

					for(unsigned int k=0;k<thisDInfo.size();k++)
					{
						if(thisDInfo[k].day==day && thisDInfo[k].upOrdown==i_UorD && (i_UorD==1 || i_UorD==-1))
						{
							same++;
							newDInfo->upOrdown = i_UorD;
							break;
						}
						else if(thisDInfo[k].day==day)
						{
							newDInfo->upOrdown = 0;
							break;
						}
					}
					comDayInfos.push_back(*newDInfo);
					delete newDInfo;
				}

				if(same >= step)
				{
					int wgt = int((same-step)/step);
					vector<string> hedge;
					hedge.push_back(stocks[i]->name);
					for(unsigned int l=0;l<edgeSets[j].size();l++)
						hedge.push_back(edgeSets[j][l]);

					sort(hedge.begin(),hedge.end(),sortVec);

					int isExist=0;
					for(unsigned int c=0;c<newEdgeSets.size();c++)
					{
						if(hedge==newEdgeSets[c])
						{
							isExist+=1;
							break;
						}
					}
					if(isExist==0)
					{
						nhedge+=1;
						newEdgeSets.push_back(hedge);
						newcomDI.push_back(comDayInfos);
						writeEdge(wgt,hedge,hgr);
					}
					vector<string>().swap(hedge);
				}
				vector<string>().swap(thisedge);
				vector<DayInfo>().swap(thisDInfo);
			}
#ifdef DEBUG
			vector<DayInfo*>().swap(comDayInfos);	//释放comDayInfos的内存
#endif

		}
	}

	cnt+=1;

	vector<vector<DayInfo>>().swap(oldcomDI);
	vector<vector<string>>().swap(edgeSets);
	createHgrByStep(newcomDI,newEdgeSets,hgr,cnt);

}

void StockHgraph::createHgr(string	outPath)
{
	vector<vector<DayInfo>> newcomDI;
	vector<vector<string>>	edgeSets;

	fstream hgr;
	hgr.open("../output/temp.hgr",ios::out);

	cout<<"2 nodes first..."<<endl;

	//先开始找出只包含两个顶点的超边
	for(unsigned int i=0;i<stocks.size()-1;i++)
	{
		Stock* stocki = stocks[i];
	
		for(unsigned int j=i+1;j<stocks.size();j++)
		{
			int same=0;

			Stock* stockj = stocks[j];
			vector<DayInfo> comDayInfos;

			for(map<int,int>::iterator iter=stocki->dayInfos.begin();iter!=stocki->dayInfos.end();iter++)
			{
				int day = iter->first;
				int i_UorD=iter->second;
				DayInfo* comDayInfo = new DayInfo();
				comDayInfo->day = day;

				if( stockj->dayInfos.find(day)!=stockj->dayInfos.end() )
				{
					int j_UorD=stockj->dayInfos[day];
		
					if( i_UorD==j_UorD && (i_UorD==1 || i_UorD==-1))
					{
						same++;
						comDayInfo->upOrdown = i_UorD;
					}
					else
						comDayInfo->upOrdown = 0;
				}
				comDayInfos.push_back(*comDayInfo);
				delete comDayInfo;
			}
			if(same >= step)
			{
				int wgt = int((same-step)/step);
				vector<string> hedge;

				hedge.push_back(stocki->name);
				hedge.push_back(stockj->name);
				edgeSets.push_back(hedge);
				newcomDI.push_back(comDayInfos);
				nhedge+=1;
				writeEdge(wgt,hedge,hgr);
				
				vector<string>().swap(hedge);
			}
#ifdef DEBUG
			vector<DayInfo*>().swap(comDayInfos);	//释放comDayInfos的内存
#endif
		}
	}

	createHgrByStep(newcomDI,edgeSets,hgr,2);

	hgr.close();

	vector<vector<DayInfo>>().swap(newcomDI);
	vector<vector<string>>().swap(edgeSets);

	fstream Hgr,temp;
	hgr.open("../output/temp.hgr",ios::in);
	Hgr.open(outPath,ios::out);
	Hgr<<nhedge<<" "<<nodeCnt-1<<" 11\n";
	char line[5000] = {0};
	int p=1;
	while(!hgr.eof())
	{
		hgr.getline(line,sizeof(line));
		for(int i=0;line[i]!='\n';i++)
		{
			Hgr<<line[i];
		}
		if( p!=nhedge )
			Hgr<<"\n";
		p+=1;
	}

	for(int i=0;i<nodeCnt-2;i++)
	{
		Hgr<<"1\n";
	}
	Hgr<<"1";
	Hgr.close();

	fstream mapfile;
	mapfile.open("../output/map.txt",ios::out);
	mapfile<<nodeMap;
	mapfile.close();
}