#include "Case_Records_Real.h"
CCase_Records_Real::CCase_Records_Real()
{

}

CCase_Records_Real::~CCase_Records_Real()
{
	std::list<CCaseRecord*>::iterator ip42;
	while (!RecordList.empty())
	{
		ip42=RecordList.begin();
		CCaseRecord* tmpRecord=*ip42;
		delete tmpRecord;
		RecordList.pop_front();
	}
	RecordList.clear();

	std::list<CCaseRecordItem*>::iterator ip;
	while (!ItemList.empty())
	{
		ip=ItemList.begin();
		CCaseRecordItem* tmpItem=*ip;
		this->tmp_delete_CCaseRecordItems+=1;
		delete tmpItem;
		ItemList.pop_front();
	}
	ItemList.clear();

}
bool CCase_Records_Real::AddRecord(CCaseRecord *pR)
{
	//assume passed record is newly constructed
	//and not a member of this collection
	//calling function cleans up on a return of false


	CCaseRecord* local_Record;
	CCaseRecordItem* local_Item;
	CCaseRecordItem* passed_Item;

	//do we already have the Record?
	std::list<CCaseRecord*>::iterator ip42;
	for(ip42=RecordList.begin();ip42!=RecordList.end();ip42++)
	//for (int i=0; i<RecordCount(); i++)
	{
		local_Record=*ip42;
		if (*local_Record == *pR)
		{
			local_Record->m_Occurrences+=1;
			//need to increment Occurrences of Items too
			std::list<CCaseRecordItem*>::iterator ip;
			for (ip=local_Record->ItemList.begin();ip!=local_Record->ItemList.end();ip++)
			//for (int y=0; y<local_Record->ItemList.GetSize();y++)
			{
				CCaseRecordItem* getItem=(*ip);
				getItem->Occurrences+=1;	
			}

			return false;
		}
	}
	
	//do we already have all the items?
	bool foundItem;
	
	std::list<CCaseRecordItem*>::iterator ip;
	int holdSize=pR->ItemList.size();
	for (int yeti=0;yeti < holdSize;yeti++)
	//for(int j=0; j<pR->ItemCount();j++) //loop over items in passed record
	{
		foundItem=false;
		ip=pR->ItemList.begin();
		passed_Item=*ip;

		pR->ItemList.pop_front();

		std::list<CCaseRecordItem*>::iterator ip2;
		for (ip2=ItemList.begin();ip2!=ItemList.end();ip2++)
		//for ( i=0; i<ItemCount(); i++) //loop over items in record list's collection of items
		{
			local_Item=(*ip2);
			if (*local_Item == *passed_Item)
			{
				foundItem=true;
				//we have already seen that item
				//delete the pointer(object) and remove the pointer from
				//the item list and add the pointer to the existing
				//item to the passed record's item list
				//and increment m_Occurrecnes of the item
				//old style, prior to list implementation
				//pR->ItemList.InsertAt(j,local_Item);
				//pR->ItemList.RemoveAt(j+1);
				
				//pR->ItemList.remove(passed_Item);
				delete passed_Item;
				tmp_delete_CCaseRecordItems+=1;

				pR->ItemList.push_back(local_Item);
				local_Item->Occurrences+=1;
				break;
			}
		}
		if (!foundItem)
		{
			ItemList.push_back(passed_Item);
			pR->ItemList.push_back(passed_Item);
		}
	}


	RecordList.push_back(pR);
	return true;
}
