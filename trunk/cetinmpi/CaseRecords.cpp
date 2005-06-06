#include "CaseRecords.h"
CCaseRecords::CCaseRecords()
{
	tmp_delete_CCaseRecordItems=0;//count of delete applied to CCaseRecordItems
	tmp_new_CCaseRecordItems=0;//count of new applied to CCaseRecordItems

}

CCaseRecords::~CCaseRecords()
{
//virtual overloaded
}
bool CCaseRecords::AddRecord()
{
	return true;
}


int CCaseRecords::RecordCount()
{

	return RecordList.size();
}

int CCaseRecords::CaseCount()
{
	int count=0;
	CCaseRecord* local_Record;

	std::list<CCaseRecord*>::iterator ip;
	for(ip=RecordList.begin();ip!=RecordList.end();ip++)
	//for (int i=0; i<RecordCount(); i++)
	{
		local_Record=*ip;
		count=count+local_Record->m_Occurrences;
	}
	
	return count;
}



//DEL bool CCaseRecords::AddRecord(CCaseRecord *pR)
//DEL {
//DEL 	CCaseRecord* local_Record;
//DEL 	CCaseRecordItem* local_Item;
//DEL 	CCaseRecordItem* passed_Item;
//DEL 
//DEL 	//do we already have the Record?
//DEL 	for (int i=0; i<RecordCount(); i++)
//DEL 	{
//DEL 		local_Record=(CCaseRecord*)RecordList.GetAt(i);
//DEL 		if (*local_Record == *pR)
//DEL 		{
//DEL 			local_Record->m_Occurrences+=1;
//DEL 			//need to increment Occurrences of Items too
//DEL 			for (int y=0; y<local_Record->ItemList.GetSize();y++)
//DEL 			{
//DEL 				CCaseRecordItem* getItem=(CCaseRecordItem*)local_Record->ItemList.GetAt(y);
//DEL 				getItem->Occurrences+=1;	
//DEL 			}
//DEL 
//DEL 			return false;
//DEL 		}
//DEL 	}
//DEL 	
//DEL 	//do we already have all the items?
//DEL 	bool foundItem;
//DEL 	for(int j=0; j<pR->ItemCount();j++) //loop over items in passed record
//DEL 	{
//DEL 		foundItem=false;
//DEL 		passed_Item=(CCaseRecordItem*)pR->ItemList.GetAt(j);
//DEL 		for ( i=0; i<ItemCount(); i++) //loop over items in record list's collection of items
//DEL 		{
//DEL 			local_Item=(CCaseRecordItem*)ItemList.GetAt(i);
//DEL 			if (*local_Item == *passed_Item)
//DEL 			{
//DEL 				foundItem=true;
//DEL 				//we have already seen that item
//DEL 				//delete the pointer(object) and remove the pointer from
//DEL 				//the item list and add the pointer to the existing
//DEL 				//item to the passed record's item list
//DEL 				//and increment m_Occurrecnes of the item
//DEL 				pR->ItemList.InsertAt(j,local_Item);
//DEL 				pR->ItemList.RemoveAt(j+1);
//DEL 				delete passed_Item;
//DEL 				//pR->ItemList.Add(local_Item);
//DEL 				local_Item->Occurrences+=1;
//DEL 				break;
//DEL 			}
//DEL 		}
//DEL 		if (!foundItem)
//DEL 		{
//DEL 			ItemList.Add(passed_Item);	
//DEL 		}
//DEL 	}
//DEL 
//DEL 
//DEL 	RecordList.Add(pR);
//DEL 	return true;
//DEL }

CCaseRecord* CCaseRecords::GetRecord(CCaseRecord* pR)
{
	CCaseRecord* local_Record;

	std::list<CCaseRecord*>::iterator ip;
	for(ip=RecordList.begin();ip!=RecordList.end();ip++)
	//for (int i=0; i<RecordCount(); i++)
	{
		local_Record=*ip;
		if (*local_Record == *pR)
		{
			return local_Record;
		}
	}
	
	return 0;
}

int CCaseRecords::ItemCount()
{
	return ItemList.size();
}

CCaseRecordItem* CCaseRecords::GetRecordItem(CCaseRecordItem &pItem)
{
	//CCaseRecordItem* local_Item;

	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=ItemList.begin();ip!=ItemList.end();ip++)
	//for (int i=0; i<ItemCount(); i++)
	{
		CCaseRecordItem* local_Item=(*ip);
		if (*local_Item == pItem)
		{
			return local_Item;
		}
	}
	
	return 0;
}



int CCaseRecords::BiggestItemValue(string &nodeName)
{
	//used to find the number of states of a node from the data
	//CCaseRecordItem* local_Item;
	int bigVal=1;
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=ItemList.begin();ip!=ItemList.end();ip++)
	//for (int i=0; i<ItemCount(); i++)
	{
		CCaseRecordItem* local_Item=(*ip);
		if (local_Item->NodeName == nodeName && local_Item->Value > (float)bigVal)
		{
			bigVal=(int)local_Item->Value;
		}
	}
	return bigVal;
}
double CCaseRecords::ExpectedValue(string &nodeName)
{
        //used to find expected value of a variable (INDEPENDENCE ASSUMED)
        double retVal=0;
        double totalOccurrences=0;
        std::list<CCaseRecordItem*>::iterator ip;
        for (ip=ItemList.begin();ip!=ItemList.end();ip++)
        //for (int i=0; i<ItemCount(); i++)
        {
                CCaseRecordItem* local_Item=(*ip);
                if (local_Item->NodeName == nodeName )
                {
                        retVal=retVal+((double)local_Item->Value)*((double)local_Item->Occurrences);
                        totalOccurrences=totalOccurrences+(double)local_Item->Occurrences;
                }
        }
        retVal=retVal/totalOccurrences;
        return retVal;
}

double CCaseRecords::Correlation(string &nodeName1, string &nodeName2)
{
        //used to find correlation of two nodes
        double retVal=0;
        std::list<CCaseRecord*>::iterator ip;
        for (ip=RecordList.begin();ip!=RecordList.end();ip++)
        //for (int i=0; i<ItemCount(); i++)
        {
                CCaseRecord* local_Record=(*ip);
                CCaseRecordItem* Item1=local_Record->GetItem(nodeName1);
                CCaseRecordItem* Item2=local_Record->GetItem(nodeName2);

                retVal=retVal+(double)local_Record->m_Occurrences * ((double)Item1->Value)*((double)Item2->Value);
        }
        retVal=retVal/CaseCount();
        return retVal;
}

double CCaseRecords::Variance(string &nodeName)
{
        //used to find variance E[X^2]-E[X]^2 of a variable (INDEPENDENCE ASSUMED)
        double retVal=0;
        double totalOccurrences=0;
        std::list<CCaseRecordItem*>::iterator ip;
        for (ip=ItemList.begin();ip!=ItemList.end();ip++)
        //for (int i=0; i<ItemCount(); i++)
        {
                CCaseRecordItem* local_Item=(*ip);
                if (local_Item->NodeName == nodeName )
                {
                        retVal=retVal+((double)local_Item->Value)*((double)local_Item->Value)*((double)local_Item->Occurrences);
                        totalOccurrences=totalOccurrences+(double)local_Item->Occurrences;
                }
        }
        retVal=retVal/totalOccurrences;
        double ev=ExpectedValue(nodeName);
        retVal=retVal-(ev)*(ev);
        return retVal;
}
