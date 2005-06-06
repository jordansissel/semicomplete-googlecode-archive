#include "Node.h"
#include <math.h>



CNode::CNode()
{
	m_MAX_STATES		=5;
	m_Selected			=0;
	m_NumOfStates       = 2;
	m_Modified          = false;
	m_EvidenceFlag      = -1;
	m_InstantiatedState = -1;
	m_NodeType			=0; //0=chance node, 1 =action node
	//idNODE_CHANCE 0  idNODE_ACTION 1
}

CNode::~CNode()
{
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=m_CaseRecord_Template.ItemList.begin();ip!=m_CaseRecord_Template.ItemList.end();ip++)
	{
		delete *ip;
	}
	m_CaseRecord_Template.ItemList.clear();

	s_Child.clear();




}

 CNode::CNode(const CNode &nodeToCopy) //copy constructor
 {
 	m_NodeName=nodeToCopy.m_NodeName;
	m_NodeNumber=nodeToCopy.m_NodeNumber;

	m_MAX_STATES		=nodeToCopy.m_MAX_STATES;
 	m_Selected			=nodeToCopy.m_Selected;
 
 	m_NumOfStates       = nodeToCopy.m_NumOfStates;
 	m_Modified          = nodeToCopy.m_Modified;
 	m_EvidenceFlag      = nodeToCopy.m_EvidenceFlag;
 	m_InstantiatedState = nodeToCopy.m_InstantiatedState;
 	m_NodeType			=nodeToCopy.m_NodeType; //0=chance node, 1 =action node
 	//idNODE_CHANCE 0  idNODE_ACTION 1 set in stdafx.h
 
	this->boolTag=nodeToCopy.boolTag;
// 	this->m_CaseRecord_Template=nodeToCopy.m_CaseRecord_Template;
	this->m_NodeCPT=nodeToCopy.m_NodeCPT;
 	this->m_NodeCPTdenum=nodeToCopy.m_NodeCPTdenum;
 	this->m_NodeCPTnum=nodeToCopy.m_NodeCPTnum;
 	this->m_NodeName=nodeToCopy.m_NodeName;
 	this->m_NodeNumber=nodeToCopy.m_NodeNumber;
 	this->m_Prob=nodeToCopy.m_Prob;
	//this->s_Child=nodeToCopy.s_Child;
 	this->s_Parent=nodeToCopy.s_Parent;
//	this->GetCaseRecord_Template();
 }
 CNode* CNode::operator= (const CNode &nodeToCopy) //copy assignment operator
 {
 	m_NodeName=nodeToCopy.m_NodeName;
	m_NodeNumber=nodeToCopy.m_NodeNumber;

 	m_MAX_STATES		=nodeToCopy.m_MAX_STATES;
 	m_Selected			=nodeToCopy.m_Selected;
 
 	m_NumOfStates       = nodeToCopy.m_NumOfStates;
 	m_Modified          = nodeToCopy.m_Modified;
 	m_EvidenceFlag      = nodeToCopy.m_EvidenceFlag;
 	m_InstantiatedState = nodeToCopy.m_InstantiatedState;
 	m_NodeType			=nodeToCopy.m_NodeType; //0=chance node, 1 =action node
 	//idNODE_CHANCE 0  idNODE_ACTION 1 set in stdafx.h
 
	this->boolTag=nodeToCopy.boolTag;
 //	this->m_CaseRecord_Template=nodeToCopy.m_CaseRecord_Template;
	this->m_NodeCPT=nodeToCopy.m_NodeCPT;
 	this->m_NodeCPTdenum=nodeToCopy.m_NodeCPTdenum;
 	this->m_NodeCPTnum=nodeToCopy.m_NodeCPTnum;
 	this->m_NodeName=nodeToCopy.m_NodeName;
 	this->m_NodeNumber=nodeToCopy.m_NodeNumber;
 	this->m_Prob=nodeToCopy.m_Prob;
	//this->s_Child=nodeToCopy.s_Child;
 	this->s_Parent=nodeToCopy.s_Parent;
//	this->GetCaseRecord_Template();
	return this; //return address of this object
 }


CCaseRecord* CNode::GetCaseRecord_Template()
{
	//delete previous template items
	//CObject* myObject;
	//for (int y=0;y<m_CaseRecord_Template.ItemList.GetSize();y++)
	//{
	//	myObject=m_CaseRecord_Template.ItemList[y];
	//	delete myObject;
	//}
	//m_CaseRecord_Template.ItemList.RemoveAll();m_CaseRecord_Template.ItemList.FreeExtra();
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=m_CaseRecord_Template.ItemList.begin();ip!=m_CaseRecord_Template.ItemList.end();ip++)
	{
		delete *ip;
	}
	m_CaseRecord_Template.ItemList.clear();
	//add my parents
	for (int i=0;i<s_Parent.ItemCount();i++)
	{
		CCaseRecordItem* newItem=new CCaseRecordItem();
		newItem->NodeName=*s_Parent[i];
		newItem->Value=0.0f;
		m_CaseRecord_Template.ItemList.push_back(newItem);
	}
	//add myself	
	CCaseRecordItem* newItem=new CCaseRecordItem();
	newItem->NodeName=m_NodeName;
	newItem->Value=0.0f;
	m_CaseRecord_Template.ItemList.push_back(newItem);

	return &m_CaseRecord_Template;

}

void CNode::AddParent(string &vParentName)
{
	s_Parent.Add(vParentName);
}

void CNode::AddChild(string &vChildName)
{
	string addString=vChildName;
	s_Child.push_front(addString);
}



void CNode::RemoveParent(string parentName)
{
	s_Parent.Remove(parentName);

}

void CNode::RemoveChild(string childName)
{
	//function removes only the child
	//from the array  s_Child is CStringArray
	//it does not recalculate the cpt of the affected child
	string removeMe=childName;
	s_Child.remove(removeMe);

	
	
}
bool CNode::operator ==(const CNode &node)
//very simplistic
{
	if (m_NodeName  == node.m_NodeName)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CNode::operator !=(const CNode &node)
//very simplistic
{
	if (m_NodeName  != node.m_NodeName)
	{
		return true;
	}
	else
	{
		return false;
	}
}
