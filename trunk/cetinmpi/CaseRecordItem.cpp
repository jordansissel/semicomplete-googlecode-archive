#include "CaseRecordItem.h"

CCaseRecordItem::CCaseRecordItem()
{
	Occurrences=1;
}
CCaseRecordItem::CCaseRecordItem(const CCaseRecordItem &passItem)//copy
{
	Occurrences=passItem.Occurrences;
	NodeName=passItem.NodeName;
	Value=passItem.Value;
}
CCaseRecordItem::CCaseRecordItem( CCaseRecordItem* passItem)//copy
{//set this one to be identical to passed object
	Occurrences=passItem->Occurrences;
	NodeName=passItem->NodeName;
	Value=passItem->Value;
	//this=&passItem;
}

void CCaseRecordItem::operator =(const CCaseRecordItem &passItem)//assignment
{
	Occurrences=passItem.Occurrences;
	NodeName=passItem.NodeName;
	Value=passItem.Value;
}

CCaseRecordItem::~CCaseRecordItem()
{

}

bool CCaseRecordItem::operator ==(CCaseRecordItem &pRI)
{
	if (pRI.NodeName  == NodeName     &&	   \
		pRI.Value     == Value                  )
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CCaseRecordItem::operator !=(CCaseRecordItem &pRI)
{
	if (pRI.NodeName  != NodeName     ||	   \
		pRI.Value     != Value                  )
	{
		return true;
	}
	else
	{
		return false;
	}
}
