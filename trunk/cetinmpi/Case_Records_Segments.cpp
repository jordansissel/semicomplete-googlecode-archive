#include "Case_Records_Segments.h"


CCase_Records_Segments::CCase_Records_Segments()
{
}

CCase_Records_Segments::~CCase_Records_Segments()
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
		delete tmpItem;
		ItemList.pop_front();
	}
	ItemList.clear();

}

bool CCase_Records_Segments::AddRecord(CCaseRecord *pR,list<CNode> *nodeList,bool IsNew)
{

	//if (!IsNew)
	//{
	//	if(GetRecord(pR)!=0)
	//	{
	//		return true;
	//	}
	//}

	//remember that passed case record may have occurences > 1
	CCaseRecord* local_Record;
	//CCaseRecordItem* local_Item;
	CCaseRecordItem* tmpItem;
//	CCaseRecordItem* passed_Item;
	CCaseRecordItem* segment_Item;

	//loop over   nodes
	std::list<CNode>::iterator ip;
	for ( ip = nodeList->begin(); ip != nodeList->end(); ip++ ) 
	//for (int y=0; y<nodeList->size();y++)
	{
		//CNode* pNode=(CNode*)nodeList->GetAt(y);
		CNode* pNode=&(*ip);
		
		if(pNode->s_Parent.ItemCount()>0 && pR->ContainsParentsAndNode(pNode->m_NodeName,pNode->s_Parent))
		{
			//construct a new segment case record
			//add all the parent items
			CCaseRecord* segment=new CCaseRecord();
			CCaseRecordItem* addRecordItem;
			for (int j=0; j < pNode->s_Parent.ItemCount(); j++)
			{
				
				addRecordItem=new CCaseRecordItem();
				tmpItem=pR->GetItem(*pNode->s_Parent[j]);
				addRecordItem->NodeName=tmpItem->NodeName;
				addRecordItem->Value=tmpItem->Value;
				addRecordItem->Occurrences=tmpItem->Occurrences;
				segment->ItemList.push_back(addRecordItem);
			}
			//now add myself
			addRecordItem=new CCaseRecordItem();
			addRecordItem->NodeName=pNode->m_NodeName;
			tmpItem=pR->GetItem(pNode->m_NodeName);
			addRecordItem->NodeName=pNode->m_NodeName;
			addRecordItem->Value=tmpItem->Value;

			addRecordItem->Occurrences=tmpItem->Occurrences;

			segment->ItemList.push_back(addRecordItem);
			segment->m_Occurrences=pR->m_Occurrences;

			//do we already have the Record?
			//then delete the newly created segment and items
			local_Record=this->GetRecord(segment);
			if (local_Record != 0 ) //if we have the record
			{
				if (local_Record->IsNew)//IMPORTANT
				{
					local_Record->m_Occurrences=local_Record->m_Occurrences+pR->m_Occurrences;
					//need to increment Occurrences of Items too
					std::list<CCaseRecordItem*>::iterator ip;
					for (ip=local_Record->ItemList.begin();ip!=local_Record->ItemList.end();ip++)
					//for (int y=0; y<local_Record->ItemList.GetSize();y++)
					{
						CCaseRecordItem* getItem=(*ip);
						//this isn't correct but isn;t used anyway
						getItem->Occurrences+=1;	
					}
				}
				//delete the newly created segment and items
				//first delete CCaseRecordItem Objects
				//**************************************
				std::list<CCaseRecordItem*>::iterator ip2;
				while (!segment->ItemList.empty())
				{
					ip2=segment->ItemList.begin();
					CCaseRecordItem* tmpItem=*ip2;	
					delete tmpItem;
					segment->ItemList.pop_front();
				}
				segment->ItemList.clear();
				//**************************************
				//**************************************
				//now delete the object
				delete segment;
			}
			else
			{
				//do we already have all the items?
				bool foundItem;
				std::list<CCaseRecordItem*>::iterator ip3;
				//for (ip3=segment->ItemList.begin();ip3!=segment->ItemList.end();ip3++)
				int holdCount=segment->ItemCount();
				for(int j=0; j<holdCount;j++) //loop over number of items in segment
				{
					foundItem=false;
					ip3=segment->ItemList.begin();
					segment_Item=(*ip3);
					segment->ItemList.pop_front();

					std::list<CCaseRecordItem*>::iterator ip4;
					for (ip4=ItemList.begin();ip4!=ItemList.end();ip4++)
					//for (int  i=0; i<ItemCount(); i++) //loop over items in record list's collection of items
					{
						CCaseRecordItem* local_Item=(*ip4);
						if (*local_Item == *segment_Item)
						{
							foundItem=true;
							//we have already seen that item
							//delete the pointer(object) and remove the pointer from
							//the item list and add the pointer to the existing
							//item to the passed record's item list
							//and increment m_Occurrecnes of the item
							//segment->ItemList.InsertAt(j,local_Item);
							//segment_Item=(CCaseRecordItem*)segment->ItemList.GetAt(j+1);
							//delete segment_Item;
							//segment->ItemList.RemoveAt(j+1);

							delete segment_Item;
							segment->ItemList.push_back(local_Item);
							local_Item->Occurrences+=1;
							break;
						}
					}
					if (!foundItem)
					{
						ItemList.push_back(segment_Item);	
						segment->ItemList.push_back(segment_Item);
					}
				}
				RecordList.push_back(segment);
				//TRACE(_T("added new permutation  \n"));
			}// end if then else on record exists in segments
		}//end if of if the node has parents
	}//end loop over nodes
	return true;
}
