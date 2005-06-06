#include "ParentList.h"

CParentList::CParentList()
{

}
CParentList::CParentList(CParentList& copyList) //copy constructor
{
	s_List=copyList.s_List;
}
CParentList::~CParentList()
{
/*	for (int i=0; i<s_List.GetSize();i++)
	{
		CString* value;
		value=&s_List[i];
		delete value;
	}*/
	s_List.clear();

}

void CParentList::Add(string &refString)
{
	string addString=refString;
	s_List.push_front(addString);
}	

void CParentList::Remove(string &refString)
{
	string removeMe=(refString);
	s_List.remove(removeMe);
}
bool CParentList::operator ==(CParentList &remoteList)
{
	if (s_List == remoteList.s_List)
	{
		return true;
	}
	else
	{
		return false;
	}

}
string* CParentList::operator [](int &i)
{
	
	int iRow=0;
	std::list<string>::iterator ip ;
	for (ip=s_List.begin();ip != s_List.end();ip++ )
	{
		if (i==iRow)
		{
			return &(*ip);
		}
		iRow++;
	}
	return 0;

}


int CParentList::ItemCount()
{
	if (s_List.empty())
	{
		return 0;
	}
	else
	{
		return s_List.size();
	}
}
CParentList* CParentList::operator= (const CParentList& copyList) //copy assignment operator
{
	s_List=copyList.s_List;
	return this;
}
