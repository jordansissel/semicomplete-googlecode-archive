#include "CaseRecord.h"
CCaseRecord::CCaseRecord()
{
	m_Occurrences=1;
	IsNew=true;
}

CCaseRecord::CCaseRecord( CCaseRecord &passRecord) //copy constructor
{
	m_Occurrences=passRecord.m_Occurrences;
	IsNew=passRecord.IsNew;
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=passRecord.ItemList.begin();ip!=passRecord.ItemList.end();ip++)
	//for (int i=0;i<passRecord.ItemCount();i++)
	{
		CCaseRecordItem* tmpItem=(*ip);//assignment construction
		ItemList.push_back(tmpItem);
	}
}
void CCaseRecord::operator =( CCaseRecord &passRecord) //assignment
{
	m_Occurrences=passRecord.m_Occurrences;
	IsNew=passRecord.IsNew;
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=passRecord.ItemList.begin();ip!=passRecord.ItemList.end();ip++)
	//for (int i=0;i<passRecord.ItemCount();i++)
	{
		CCaseRecordItem* tmpItem=(*ip);//assignment construction
		ItemList.push_back(tmpItem);
	}
}
CCaseRecord::~CCaseRecord()
{
	//parent collection cleans up item objects
	//or cleaned up by record creator
	//ItemList.clear();
}
bool CCaseRecord::operator ==(CCaseRecord &pR)
{
	if (pR.ItemCount() != ItemCount() )
	{
		return false; //quick return to avoid looping over objects
	}
	
	CCaseRecordItem* tmpItem_Local;
	CCaseRecordItem* tmpItem_Remote;
	bool foundit;

	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=ItemList.begin();ip!=ItemList.end();ip++)
	//for (int i=0; i< ItemCount() ; i++)
	{
		tmpItem_Local=(*ip);
		foundit=false;
		std::list<CCaseRecordItem*>::iterator ip2;
		for (ip2=pR.ItemList.begin();ip2!=pR.ItemList.end();ip2++)
		//for (int j=0; j< pR.ItemCount() ; j++)
		{
			tmpItem_Remote=(*ip2);
			if ( *tmpItem_Local == *tmpItem_Remote )
			{
				foundit=true;
				break;
			}
		}
		if (!foundit) {return false;}
	}
	return true;

}

int CCaseRecord::ItemCount()
{
	return ItemList.size();
}

bool CCaseRecord::Contains(CCaseRecord *caserecordSegment)
{
	if (caserecordSegment->ItemCount() > ItemCount() )
	{//segment cannot be longer than whole record
		return false; //quick return to avoid looping over objects
	}
	
	//CCaseRecordItem* tmpItem_Local;
	//CCaseRecordItem* tmpItem_Remote;
	bool foundit;

	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=caserecordSegment->ItemList.begin();ip!=caserecordSegment->ItemList.end();ip++)
	//for (int i=0; i< caserecordSegment->ItemCount() ; i++) //loop over node states in then segment
	{
		CCaseRecordItem* tmpItem_Remote=(*ip);
		foundit=false;
		std::list<CCaseRecordItem*>::iterator ip2;
		for (ip2=ItemList.begin();ip2!=ItemList.end();ip2++)
		//for (int j=0; j< ItemCount() ; j++) //loop over record node states
		{
			CCaseRecordItem* tmpItem_Local=(*ip2);
			if ( *tmpItem_Local == *tmpItem_Remote )
			{
				foundit=true;
				break;
			}
		}
		if (!foundit) {return false;}
	}
	return true;
}

bool CCaseRecord::ContainsParents(CParentList *pParentList)
{
	if (pParentList->ItemCount() > ItemCount() )
	{//parent list cannot be longer than whole record
		return false; //quick return to avoid looping over objects
	}
	
	//string* tmpParent_Local;
	//string tmpParent_Remote;
	bool foundit;


	std::list<string>::iterator p ;
	for (p = pParentList->s_List.begin();p != pParentList->s_List.end();p++ )
	{
		string& tmpParent_Remote=*p;
		foundit=false;
		std::list<CCaseRecordItem*>::iterator ip;
		for (ip=ItemList.begin();ip!=ItemList.end();ip++)
		//for (int j=0; j< ItemCount() ; j++) //loop over parent names in record
		{
			CCaseRecordItem* getItem=(*ip);
			string& tmpParent_Local=getItem->NodeName;
			if ( tmpParent_Local == tmpParent_Remote )
			{
				foundit=true;
				break;
			}
		}
		if (!foundit) {return false;}

	}

/*	for (int i=0; i< pParentList->ItemCount() ; i++) //loop over parent names passed
	{
		tmpParent_Remote=&(pParentList->s_List.GetAt(i));
		foundit=false;
		for (int j=0; j< ItemCount() ; j++) //loop over parent names in record
		{
			CCaseRecordItem* getItem=(CCaseRecordItem*)(ItemList.GetAt(j));
			tmpParent_Local=&getItem->NodeName;
			if ( *(tmpParent_Local) == *tmpParent_Remote )
			{
				foundit=true;
				break;
			}
		}
		if (!foundit) {return false;}
	} deprecated by stl list usage 8/14/02*/
	return true;
}

CCaseRecordItem* CCaseRecord::GetItem(string &NodeName)
{
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=ItemList.begin();ip!=ItemList.end();ip++)
	//for (int i=0; i<ItemList.GetSize();i++)
	{
		CCaseRecordItem* cri=(*ip);
		if((string)(cri->NodeName)==NodeName)
		{
			return cri;
		}
	}
	return 0;
}

bool CCaseRecord::ContainsParentsAndNode(string& NodeName,CParentList& pParentList)
{
	if (pParentList.ItemCount() >= ItemCount() )
	{//parent list cannot be longer than whole record
		return false; //quick return to avoid looping over objects
	}

	for (int i=0; i< pParentList.ItemCount() ; i++) //loop over parent names passed
	{
		string sparent=*pParentList[i];	
		CCaseRecordItem* getItem=GetItem(sparent);
		if ( getItem==0 )
		{
			return false;
		}

	}
	if (GetItem(NodeName)==0) 
	{
		return false;
	}
	return true;
}
