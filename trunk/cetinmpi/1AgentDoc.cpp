// 1AgentDoc.cpp : implementation file
//

#include <fstream>
#include "1AgentDoc.h"
#include <math.h>
#include "CaseRecord.h"
#include "CaseRecordItem.h"
#include <string>
#include <list>
#include <vector>
using std::vector;
using std::list;
using std::string;


/////////////////////////////////////////////////////////////////////////////
// C1AgentDoc


//C1AgentDoc::C1AgentDoc(char* node_sizes_filename,char* node_names_filename)
C1AgentDoc::C1AgentDoc(char * FileName)
{

	expected= CMatrix(6,1);	
	caseCounter=0;
	DISTANCE_TYPE  = 2;//0=kl 1=eu 2=logl
	ACCURACY       = 100;
	COMPLEXITY     = 1;
	SCORE_TYPE     = 2;//0=mdl 1=bayes 2=herskovits
//	IsCyclic       = FALSE;
	SEARCH_METHOD  = 0;//pso 
	
	m_AgentName = "pso_dude";
	

	this->LoadNetwork(FileName);
	//this->LoadNetwork(node_sizes_filename,node_names_filename);
}

C1AgentDoc::~C1AgentDoc()
{
	
	//clean up objects
	//zero out nodes and arrows and decoder
	//***************************************************
	ClearHeapObjects();


}



/////////////////////////////////////////////////////////////////////////////
// C1AgentDoc diagnostics


CNode& C1AgentDoc::AddNode()
{
	//Create a new CNode object

	const CNode pNode = CNode();
	m_oaNodes.push_back(pNode);
	//push_back calls the copy constructor
	//delete pNode;


	return (m_oaNodes.back());
}


int C1AgentDoc::GetArrowCount()
{
	//return the arrow array count
	return m_oaArrows.size();
}




CArrow * C1AgentDoc::GetArrow(int nIndex)
{
	int iRow=0;
	std::list<CArrow>::iterator ip;
	for ( ip = m_oaArrows.begin(); ip != m_oaArrows.end(); ip++ ) 
	{
		if (nIndex==iRow)
		{
			return &(*ip);
		}
		iRow++;
	}
	return 0;
	
}

CNode* C1AgentDoc::GetNode(int nIndex)
{
	//Return a pointer to the node object at the specified location in the object array
	int iRow=0;
	std::list<CNode>::iterator ip;
	for ( ip = m_oaNodes.begin(); ip != m_oaNodes.end(); ip++ ) 
	{
		if (nIndex==iRow)
		{
			return &(*ip);
		}
		iRow++;
	}
	return 0;
}
int C1AgentDoc::GetNodeCount()
{
	//return the array count
	return m_oaNodes.size();
}

void C1AgentDoc::RemoveArrow(CArrow& aArrow,bool SoftRemove)
//i and j are 1-based indices
//i was tail node number
//j was head node number
{
	string TailNodeName=aArrow.s_TailNodeName;
	string HeadNodeName=aArrow.s_HeadNodeName;

	string headnodename=HeadNodeName;


	m_oaArrows.remove(aArrow);

	//Deleting the corresponding Child and Parent lists.
	CNode * pNode_i = GetNode(TailNodeName); //tail node
	CNode * pNode_j = GetNode(HeadNodeName); //head node

	int i,j; //backward compat can be removed later when not needed
	i=pNode_i->m_NodeNumber;
	j=pNode_j->m_NodeNumber;

	int k, l;
	k = pNode_i->s_Child.size(); //number of children had by tailnode
	l = pNode_j->s_Parent.ItemCount();//number of parents had by headnode
	pNode_i->s_Child.remove(headnodename);
	pNode_j->s_Parent.Remove(TailNodeName);
	
	if(!SoftRemove)
	{
		LookForNewSegmentRecords();
		CreateCPT(HeadNodeName);
	}

	//-------
	//InitStateMatrices(); //EDIT re-enable this
	//UpdateNetwork();

	//ReCalculate the CPT for the child node.
//	if (HasWorldData())  //EDIT
	{
//		pNode_j->CreateNodeCPT(this); //Recalculate the values in the CPT according to the data.
		//deprecated 5/6/02
	}

}

C1AgentDoc* C1AgentDoc::GetDoc()
{
	return this;
}


bool C1AgentDoc::OnNoRelation(string node1_name, string node2_name)
{
	
	CNode* pNode1;
	CNode* pNode2;

	pNode1=GetNode(node1_name);
	pNode2=GetNode(node2_name);

	int i;
	for ( i=0; i < pNode1->s_Parent.ItemCount() ; ++i)
	{
		if ((pNode2->m_NodeName) == *(pNode1->s_Parent[i]) )
		{
		//	MessageBox("The link exists");
			return false;
			break;
		}
	}		
	
	for (i=0; i < pNode2->s_Parent.ItemCount() ; ++i)
	{
		if ((pNode1->m_NodeName) == *(pNode2->s_Parent[i]) )
		{
		//	MessageBox("The link exists");
			return false;
			break;
		}
	}		

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// C1AgentDoc commands


bool C1AgentDoc::LoadNetwork(char* FileName)
//bool C1AgentDoc::LoadNetwork(char* node_sizes_filename,char* node_names_filename)
{

	ClearHeapObjects();

	ReadNodesFrom_File(FileName);
	ReadCasesFrom_File(FileName);

	//ReadNodesFrom_File(node_sizes_filename,node_names_filename);
	//ReadCasesFrom_File();


	//need to initialize m_NumOfStates for each variable/node
	//initialize m_Prob
	std::list<CNode>::iterator ip;

	for ( ip = m_oaNodes.begin(); ip != m_oaNodes.end(); ip++ ) 
	//t = (*ip);				
	//for ( j=0; j < m_oaNodes.size(); j++)
	{
		CNode& rNode=(*ip);
		CNode* pNode=&rNode;
		pNode->m_NumOfStates=m_CaseRecords.BiggestItemValue(pNode->m_NodeName)+1;
		pNode->m_Prob=CreateNodeProbability(pNode->m_NodeName);

	}//end for loop over fields


		//ReadArrowsFrom_File();
	
	//initialize m_PermutationSegments
	// I need this
	//add arrow was soft executed above
	//*************************************************
	std::list<CCaseRecord*>::iterator ip43;
	for (ip43=m_CaseRecords.RecordList.begin();ip43!=m_CaseRecords.RecordList.end();ip43++)
	{
		CCaseRecord* pR=*ip43;
		m_PermutationSegments.AddRecord(pR,&m_oaNodes,true);
		if(m_PermutationSegments.CaseCount()>20)
		{
			int red=42;
		}

	}
	//*************************************************
	//*************************************************

	this->CalFirstLevelProbs();
	//this->UpdateNetwork();
	string str="Password_All_Nodes_4242";
	//special text pass forces CreateCPT to recalculate all cpts

	CreateCPT(str);

	return true;

}

bool C1AgentDoc::Valid_Network()
{
	int l = m_oaNodes.size();

	for (int i=0; i < l; i++)
	{
		if (GetNode(i)->s_Parent.ItemCount() > 6) //IF NO PARENTS 
		{
		   // printf(" Node has more than 6 parents");
			return false;
		}
	}

	return true;

}




bool C1AgentDoc::AddArrow(string TailNodeName, string HeadNodeName,bool SoftAdd)
{
	//SoftAdd == true means that we don't look for new permutation segments
	//may add other features later for PSO optimization
	if (TailNodeName==HeadNodeName)
	{	
		return false;
	}


	CNode* pNodeT=GetNode(TailNodeName);
	CNode* pNodeH=GetNode(HeadNodeName);
	
	//Create a new Arrow object (adding to list class template will use copy constructor
	CArrow aArrow =  CArrow();
	CArrow* pArrow=&aArrow;
	
	string parentName=TailNodeName;
	string childName=HeadNodeName;

	//note also that m_Parent and m_Child are 0 based in node numbers
	if (OnNoRelation(TailNodeName, HeadNodeName))
	{

		pArrow->s_HeadNodeName=HeadNodeName;
		pArrow->s_TailNodeName=TailNodeName;


	}
	else
	{
		//delete pArrow;
		pArrow = 0;
	}
	
	if (pArrow!=0)
	{
		m_oaArrows.push_back(aArrow);
		pNodeH->AddParent(parentName);
		pNodeT->AddChild(childName);
	}
	
	if (IsNetworkCyclic_new(&m_oaArrows) && pArrow!=0) 
	{
		pNodeH->RemoveParent(parentName);
		pNodeT->RemoveChild(childName);
		m_oaArrows.pop_back();
		//m_oaArrows.remove(aArrow);
		//delete pArrow;
		pArrow = 0;
	}
	if(pArrow!=0 )//add permutation segments and reform cpt table of child
	{
		if(!SoftAdd)
		{
			LookForNewSegmentRecords();
			CreateCPT(pArrow->s_HeadNodeName);
			UpdateNetwork();
			tmp_NetworkScore=CalculateNetworkScore(0);
			tmp_BestFitness=(1.0/tmp_NetworkScore);
		}

	}
	if (pArrow!=0)
	{
		return true;
	}else
	{
		return false;
	}
}

CMatrix* C1AgentDoc::CreateCPT(string nodeName )
{
	//entry into this funtion assumes we have looked for
	//record segments for the passed nodes parent configuration
	int m,column;
	//if called with reserved nodename of "Password_All_Nodes_4242"
	//then create cpt for all nodes recursively
	if(nodeName=="Password_All_Nodes_4242")
	{
		//printf("Calculating CPTs for the network\n");
		for (int e=0;e<m_oaNodes.size();e++)
		{
	//		printf("Parent size %d\n", GetNode(e)->s_Parent.ItemCount());
			CreateCPT(GetNode(e)->m_NodeName);
		}
		return 0;
	}

	CNode* pNode=GetNode(nodeName);
	CCaseRecord* pR=pNode->GetCaseRecord_Template();

	CMatrix holdCPT_Column(pNode->m_NumOfStates,1);

	if (pNode->s_Parent.ItemCount()==0)
	{
		pNode->m_NodeCPT=pNode->m_Prob;
		return &pNode->m_Prob;
		//return 0;
	}

	//set new dimensions and fill with equal weights
	int ppp=ParentPermutations(nodeName);
	int num=pNode->m_NumOfStates;
	//the regular cpt is filled with fractions
	//the number cpt is filled with zeros
	pNode->m_NodeCPT.ReInit(num,ppp);
	pNode->m_NodeCPTnum.ReInit(num,ppp);//filled originally in CreateNodeProbability
	pNode->m_NodeCPTnum.OnZero();
	//loop over permutation segments
	std::list<CCaseRecord*>::iterator ip43;
	for(ip43=m_PermutationSegments.RecordList.begin();  ip43!=m_PermutationSegments.RecordList.end();ip43++ )
	{ //loop over segment records
		CCaseRecord* pRealSegment=*ip43;
		if (pRealSegment->ContainsParentsAndNode(nodeName,pNode->s_Parent))
		{
			//construct the segment set template equal to real segment
			//set segment occurrences equal as well
			std::list<CCaseRecordItem*>::iterator ip;
			for (ip=pR->ItemList.begin();ip!=pR->ItemList.end();ip++)
			//for (int jj=0; jj < pR->ItemCount() ; jj++)
			{
				CCaseRecordItem* tmpItem;
				CCaseRecordItem* pooh=(*ip);
				string segNodeName=pooh->NodeName;
				tmpItem=pRealSegment->GetItem(segNodeName);
				(*ip)->Value=tmpItem->Value;
			}
			pR->m_Occurrences=pRealSegment->m_Occurrences;

			CCaseRecordItem* pI= pR->GetItem(pNode->m_NodeName);
			//pI is the recordItem of the child node in the template
			//column calculation (column of cpt table)
			//same as row of supermatrix
			m=1;
			column=0; 
			for (int q=pNode->s_Parent.ItemCount()-1;q>-1;q--)
			{
				string passName=*(pNode->s_Parent[q]);
				CNode* qNode=GetNode(passName);
				CCaseRecordItem* pI2=pR->GetItem(qNode->m_NodeName);
				int incrementColumn=column+(int)(pI2->Value)*m;
				column=incrementColumn;
				int yy=m*qNode->m_NumOfStates;
				m=yy;
			}
			
			int setRow=(int)pI->Value;
			int setCol=0;
			float get_column_total;
			
			holdCPT_Column=GetCPTColumn(nodeName,pR,get_column_total);//pR is template segment

			for (int r=0;r<holdCPT_Column.m_row;r++)
			{
				pNode->m_NodeCPT.SetElement(r,column,holdCPT_Column.GetElement(r,0));
				pNode->m_NodeCPTnum.SetElement(r,column,holdCPT_Column.GetElement(r,0)*get_column_total);
				if(holdCPT_Column.GetElement(r,0)*get_column_total<0.9 && holdCPT_Column.GetElement(r,0)!=0.0)
				{
					int hh=98;
				}
			}
							
		}
	}
	//GetNode(node)->m_NodeCPTdenum = BasicCPT; // For updating CPT while  simulating

	return &pNode->m_NodeCPT;

}

bool C1AgentDoc::HasWorldData()
{
	
	
	if (m_CaseRecords.ItemCount() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
	
/*	
	CDaoRecordset rs(m_pDB);
	CString myWorldData=_T("World_") + m_AgentName;
	rs.Open(dbOpenTable,myWorldData,0);
	if (!rs.IsEOF())
	{
		return true;
	}
	else
	{
		return false;
	}
	if (rs.IsOpen())
	{
		rs.Close();
	}
*/
}



int C1AgentDoc::MultiplicityOfStates()
{
	int nodeCount = m_oaNodes.size();       // The number of nodes in the network
	int g=1;

	for (int node = 0; node < nodeCount; node++)
	{
		g=g*GetNode(node)->m_NumOfStates;
	}
	return g;

}



void C1AgentDoc::ModifiedFlagChild(string NodeName)
{

	//Set Modified flag for this node to FALSE;
	CNode* pNode=GetNode(NodeName);
	pNode->m_Modified = false;

	std::list<string>::iterator ip;
	for ( ip = pNode->s_Child.begin(); ip != pNode->s_Child.end(); ip++ ) 
	//for (int i=0; i < pNode->s_Child.size(); i++)
	{
		string& childname=(*ip);
//		(GetNode(childList[i])->m_Modified) = false;
		ModifiedFlagChild(childname);
	}

}



void C1AgentDoc::UpdateNetwork_directCPTlookups() 
{
//	return;
	int huh=88;
	int nodeCount = m_oaNodes.size();       // The number of nodes in the network
	int upt=0;


	for (int y=0; y < nodeCount; y++)
	{
		if (GetNode(y)->m_Modified == false) 
		//if (1==1) 
		{
			CNode *pCurNode = GetNode(y);

			
			int		n_sCurParent = pCurNode->s_Parent.ItemCount();
			bool	parentModFlag= true;        

			if (n_sCurParent!= 0)
			{

				//for (int z=0; z < n_sCurParent; z++)
				//	if (GetNode(*pCurNode->s_Parent[z])->m_Modified == false)
				//		parentModFlag = false;
						
					pCurNode->m_Prob.OnZero();
					//This portion updates the selected node
					CMatrix prntProb;

					prntProb = Parents(pCurNode->m_NodeName);
					int high=98;
					//Calculation of the new probabilities for the child nodes.
					//pCurNode->m_Prob = (pCurNode->m_NodeCPT)*prntProb;
					//above is rplaced by new calculation
						std::list<CCaseRecord*>::iterator ip43;
						for(ip43=m_PermutationSegments.RecordList.begin();  ip43!=m_PermutationSegments.RecordList.end();ip43++ )
						//for (int j=0; j<m_PermutationSegments.RecordCount();j++)
						{ //loop over segment records
							CCaseRecord* pRealSegment=*ip43;
							if (pRealSegment->ContainsParentsAndNode(pCurNode->m_NodeName,pCurNode->s_Parent))
							{

								//use node segment template to construct test record
								CCaseRecord* pR=pCurNode->GetCaseRecord_Template();
								//construct the segment set template equal to real segment
								std::list<CCaseRecordItem*>::iterator ip;
								for (ip=pR->ItemList.begin();ip!=pR->ItemList.end();ip++)
								//for (int jj=0; jj < pR->ItemCount() ; jj++)
								{
									CCaseRecordItem* tmpItem;
									string segNodeName=(*ip)->NodeName;
									tmpItem=pRealSegment->GetItem(segNodeName);
									(*ip)->Value=tmpItem->Value;
								}

								CCaseRecordItem* pI= pR->GetItem(pCurNode->m_NodeName);
								//pI is the recordItem of the child node in the template
								//row calculation (row of 'super matrix')
								int m=1;
								int row=0;
								for (int q=pCurNode->s_Parent.ItemCount()-1;q>-1;q--)
								{
									string passName=*(pCurNode->s_Parent[q]);
									CNode* qNode=GetNode(passName);
									CCaseRecordItem* pI2=pR->GetItem(qNode->m_NodeName);
									row=row+(int)(pI2->Value)*m;
									m=m*qNode->m_NumOfStates;
								}
								int setRow=(int)pI->Value;
								int setCol=0;
								float setValue=pCurNode->m_Prob(setRow,setCol)+GetCPT(pCurNode->m_NodeName,(int)(pI->Value),pR)*prntProb(row,0);
								pCurNode->m_Prob.SetElement(setRow,setCol,setValue);
							
							}
						}
						//check for zeros in all elements of m_Prob
						//this happen when no case data to do calculation
						bool allZeros=true;
						for(int i=0;i<pCurNode->m_NumOfStates;i++)
						{
							if(pCurNode->m_Prob.GetElement(i,0)!=0.0)
							{
								allZeros=false;
								break;
							}
						}
						if(allZeros)
						{
							for(int i=0;i<pCurNode->m_NumOfStates;i++)
							{
								pCurNode->m_Prob.SetElement(i,0,1.0f/(float)pCurNode->m_NumOfStates);
							}
						}
					//end new calculation
					pCurNode->m_Modified = true;
			}
			else
				pCurNode->m_Modified = true;    // This directly modifies the first level nodes 
		}
	}	

}
CMatrix C1AgentDoc::Parents(string NodeName)
{
	CNode *pNode = GetNode(NodeName);
	int q=0;
	string tmpString=*pNode->s_Parent[q];
	CNode *pFirstParent = GetNode(tmpString);
	CMatrix    parents;
	parents = pFirstParent->m_Prob;

	int l = pNode->s_Parent.ItemCount();

	if (l > 1)
	{

		for (int i=1; i < l; i++)
		{
			CNode *pAParent = GetNode(*pNode->s_Parent[i]);

			parents = parents.Supermultiply(pAParent->m_Prob);

		}

		return parents;
	}
	else
	{
		return parents;
	}
}
void C1AgentDoc::Inference(string  NodeName)
{
	ForwardInference( NodeName);
	//ForwardInference_new( NodeName);
	BackwardInference( NodeName);
}

void C1AgentDoc::ForwardInference(string  NodeName)
 {
 	CNode* pInferingNode=GetNode(NodeName);
 	
 	int l = pInferingNode->s_Child.size();
 	int k = pInferingNode->s_Parent.ItemCount();
 

	std::list<string>::iterator ip;
	for ( ip = pInferingNode->s_Child.begin(); ip != pInferingNode->s_Child.end(); ip++ ) 
	//for (int i=0; i < l; i++) //loop over the children of this node
 		//through which evidence is passing
 	{
		string& childname=(*ip);
 		if ((pInferingNode->m_EvidenceFlag == -2) || pInferingNode->m_EvidenceFlag > 0)  // this part may not be needed ||(m_EvidenceFlag != m_Parent[i]))
 		{
 			CNode	*pNode = this->GetNode(childname);
 			//pNode is the current child being considered
 			CMatrix HoldParentsMatrix=this->Parents(pNode->m_NodeName);
 			
 			//HoldParentsMatrix has ordering information based
 			//on indexes in s_Parent, I think we can work with this
 			pNode->m_Prob = (pNode->m_NodeCPT) * HoldParentsMatrix;
 
 			pNode->m_EvidenceFlag = pInferingNode->m_NodeNumber;
 
 			Inference(pNode->m_NodeName); //continue the propagation of evidence
 		}
 
 	}
 }
void C1AgentDoc::ForwardInference_new(string  NodeName)
{
	CNode* pInferingNode=GetNode(NodeName);
	
	int l = pInferingNode->s_Child.size();
	int k = pInferingNode->s_Parent.ItemCount();

	//for (int i=0; i < l; i++) 
	std::list<string>::iterator ip;
	for ( ip = pInferingNode->s_Child.begin(); ip != pInferingNode->s_Child.end(); ip++ ) 
		//loop over the children of this node
		//through which evidence is passing
	{
		string& childname=(*ip);
		if ((pInferingNode->m_EvidenceFlag == -2) || pInferingNode->m_EvidenceFlag > 0)  // this part may not be needed ||(m_EvidenceFlag != m_Parent[i]))
		{
			//CNode	*pNode = this->GetNode(pInferingNode->s_Child.GetAt(i));
			CNode	*pNode = this->GetNode(childname);
			//pNode is the current child being considered
			CMatrix HoldParentsMatrix=this->Parents(pNode->m_NodeName);
			pNode->m_Prob.OnZero();
			//HoldParentsMatrix has ordering information based
			//on indexes in s_Parent, I think we can work with this
			//pNode->m_Prob = (pNode->m_NodeCPT) * HoldParentsMatrix;
			std::list<CCaseRecord*>::iterator ip43;
			for(ip43=m_PermutationSegments.RecordList.begin();  ip43!=m_PermutationSegments.RecordList.end();ip43++ )
			//for (int j=0; j<m_PermutationSegments.RecordCount();j++)
			{ //loop over segment records

				//handle passing of actual permutation segment right in getcpt
				//may not need to call getcpt since we are looping over actual segmants here

				//for now i'll just make a copy of the case record

				
				CCaseRecord* pRealSegment=*ip43;
				
				
				if (pRealSegment->ContainsParentsAndNode(pNode->m_NodeName,pNode->s_Parent))
				{

					//LETS  use node segment template
					CCaseRecord* pR=pNode->GetCaseRecord_Template();
					//construct the segment
					std::list<CCaseRecordItem*>::iterator ip;
					for (ip=pR->ItemList.begin();ip!=pR->ItemList.end();ip++)
					//for (int jj=0; jj < pR->ItemCount() ; jj++)
					{
						CCaseRecordItem* tmpItem;
						string segNodeName=(*ip)->NodeName;
						tmpItem=pRealSegment->GetItem(segNodeName);
						(*ip)->Value=tmpItem->Value;
					}

					CCaseRecordItem* pI= pR->GetItem(pNode->m_NodeName);
					//row calculation (row of 'super matrix')
					int m=1;
					int row=0;
					for (int q=pNode->s_Parent.ItemCount()-1;q>-1;q--)
					{
						string passName=*(pNode->s_Parent[q]);
						CNode* qNode=GetNode(passName);
						CCaseRecordItem* pI2=pR->GetItem(qNode->m_NodeName);
						row=row+(int)(pI2->Value)*m;
						m=m*qNode->m_NumOfStates;
						if(row==-1)
						{
							int h7=90;
						}
					}
					int setRow=(int)pI->Value;
					int setCol=0;
					float cptVal=GetCPT(pNode->m_NodeName,(int)(pI->Value),pR);

/*					CString parentString=_T("");
					for (int r=0;r<pNode->s_Parent.ItemCount();r++)
					{
						CString strValue;
						strValue.Format("%f",pR->GetItem(*pNode->s_Parent[r])->Value);
						parentString=parentString+ _T(" ") + *pNode->s_Parent[r]+ _T(" ") + strValue;
					}
					TRACE(_T("parents=  %s \n"),parentString);
*/
					float setValue=pNode->m_Prob(setRow,setCol)+cptVal*HoldParentsMatrix(row,0);
					pNode->m_Prob.SetElement(setRow,setCol,setValue);

				}
			}
			//check for zeros in all elements of m_Prob
			//this happen when no case data to do calculation
			bool allZeros=true;
			for(int i=0;i<pNode->m_NumOfStates;i++)
			{
				if(pNode->m_Prob.GetElement(i,0)!=0.0)
				{
					allZeros=false;
					break;
				}
			}
			if(allZeros)
			{
				for(int i=0;i<pNode->m_NumOfStates;i++)
				{
					pNode->m_Prob.SetElement(i,0,1.0f/(float)pNode->m_NumOfStates);
				}
			}


			
			pNode->m_EvidenceFlag = pInferingNode->m_NodeNumber;

			Inference(pNode->m_NodeName); //continue the propagation of evidence
		}

	}
}
void C1AgentDoc::BackwardInference(string  NodeName)
{

}




void C1AgentDoc::RemoveAllArrows(bool SoftRemove)
{
	//int ubound=m_oaArrows.size();
	while(!m_oaArrows.empty())
	{
		RemoveArrow(m_oaArrows.front(),SoftRemove);
	}
	CalFirstLevelProbs();

	if(SoftRemove)
	{
		string str="Password_All_Nodes_4242";
		CreateCPT(str);
	}

}
float C1AgentDoc::CalculateNetworkScore(int intVal)
{
	//intVal=0 means caculate normally
	//intVal=0 means calculate single arc score

	//Local variables
	//int length = caseTable.m_row;
	int nodeCount = m_oaNodes.size();

	double likelihood = 1.0;
	double sum_Likelihood = 0.0;
	double act_Likelihood = 0.0;
	double node_Likelihood= 0.0;
	double distance = 0.0;
	double networkScore=0.0;

	float NumberOfCaseRecords=(float)m_CaseRecords.RecordCount();
	float TotalCases=(float)m_CaseRecords.CaseCount();
	//float NumberOfCaseRecords=(float)m_CaseRecords.CaseCount(); dep 5/6/02
	//case records are unique cases with counter for number of
	//occurences of each unique case
	if (SCORE_TYPE == 0) //MDL
	{
	  //Calculation of the likelihood for the DATA 
	  //or Distance between the true distribution and the network's distribution
		
		
	  //for(int i=0; i < length; i++) //loop over rows of world data deprecated 5/6/02
		std::list<CCaseRecord*>::iterator ip42;
		for(ip42=m_CaseRecords.RecordList.begin();  ip42!=m_CaseRecords.RecordList.end();ip42++ )
		//for(int i=0; i < NumberOfCaseRecords; i++) //loop over case records
		{
		CCaseRecord* pRecord;
		pRecord=*ip42;
		  //for(int t=0;t<pRecord->m_Occurrences;t++) //duplicate case records consideration
			{
				//calculate likelihood (from conditional probabilities)
			
				std::list<CNode>::iterator ip2;
				for ( ip2 = m_oaNodes.begin(); ip2 != m_oaNodes.end(); ip2++ ) 
				//for (int j=0; j < nodeCount; j++) //loop over nodes
				{
					//CNode& rNode=(*ip2);
					//node_Likelihood2=CalculateLikelihood(i,j); //deprecated 5/6/02
					node_Likelihood=CalculateLikelihood(*ip2,pRecord);
					//node_Likelihood=.02;
					//if (node_Likelihood!=0.0)
					//{
						likelihood = likelihood*(node_Likelihood);  // multiplying corresponding node likelihoods.
					//}
					//TRACE(_T("node_Likelihood  %f node %s record %d \n"),node_Likelihood ,pNode->m_NodeName,i);
				}

				if (pRecord !=0  && NumberOfCaseRecords!=0)
				{
					act_Likelihood = (float)pRecord->m_Occurrences/(float)TotalCases; //NEW STYLE
				}
				else
				{
					act_Likelihood=0.0f;
				}

				switch(DISTANCE_TYPE)
				{
				case 0:
						//Difference (KL method) between the actual likelihood and the network likelihood
						distance = -act_Likelihood*(log(act_Likelihood) - log(likelihood));
						break;
				case 1:
						//Diference (Euclidean) between the actual likelihood and the network likelihood
						distance = -(act_Likelihood - likelihood)*(act_Likelihood - likelihood);
						break;
				case 2:
						//Loglikelihood method
						distance = log(likelihood);
						break;
				}

				//This is the distance between the actual distribution and the network.
				sum_Likelihood = sum_Likelihood + pRecord->m_Occurrences*distance;
				//pRecord->m_Occurrences*distance above eliminates the need to
				//loop ovewr duplicate records in case records with multiple occurences
				likelihood = 1.0;							    // re-initiating the likelihood for the next case.
				//sum_Likelihood = sum_Likelihood + likelihood; // Summing the likelihoods of each case.
		  }//multiple occurrences loop
	  }//records loop
	}//if score type 0 mdl
	
	else if (SCORE_TYPE == 1) //bayes
	{
		for (int j=0; j < nodeCount; j++)
		{
			node_Likelihood = BayesScore(GetNode(j));
			sum_Likelihood = node_Likelihood + sum_Likelihood;
		}
	}
	else if (SCORE_TYPE == 2) //Hersokovits
	{
		sum_Likelihood=0.0;
		if(intVal==0){ //calculate normally
			for (int j=0; j < nodeCount; j++)
			{
				node_Likelihood = Herskovits(GetNode(j));
				sum_Likelihood = node_Likelihood + sum_Likelihood;//log sense
			}
		}
		else if(intVal==2){//return correlation coef
			for (int j=0; j < nodeCount; j++)
                        {
                                if(GetNode(j)->s_Parent.ItemCount()==1){
                                        node_Likelihood = JasonSingleArcScore(GetNode(j));
					 return  fabs(CorrelationCoefficient);
                                }
                        }
	
		}	
		else{//single arc calculation
			for (int j=0; j < nodeCount; j++)
                        {
				if(GetNode(j)->s_Parent.ItemCount()==1){
                                	node_Likelihood = JasonSingleArcScore(GetNode(j));
                                	sum_Likelihood = node_Likelihood + sum_Likelihood;//log sense
					break;
				}
                        }

		}
	}

	tmp_SumLikelihood=sum_Likelihood;

	//Calculation of the network length (complexity)
	int     paramLength = 0;
	int     node_Length = 0;
	float   networkLength;
	float   numOfArcs = 0;
 	float   connectivity   = 0.0;
	//The length of the parameters in the network
	for (int j=0; j < nodeCount; j++)
	{
		node_Length = NodeLength(GetNode(j)->m_NodeName); 

		paramLength = paramLength + node_Length;
	}

	paramLength = (log(float(TotalCases))/2.0)*paramLength;

	//Description of the network's connectivity.
	numOfArcs = float(m_oaArrows.size());
	connectivity = numOfArcs*log(float(nodeCount));

	//Total network length;
	networkLength = float(paramLength) + connectivity;

	//////////////////////////////////////////////////
   
	//Networks total description length of the system; To minimize
	if (SCORE_TYPE == 0) //MDL
	{
		switch(DISTANCE_TYPE)
		{
			case 0:
			  // KL Distance
			  sum_Likelihood = (ACCURACY )*1000*sum_Likelihood - COMPLEXITY*networkLength;
			  break;
			 case 1:
			  // Euclidean Distance
			  sum_Likelihood = (ACCURACY )*100000*sum_Likelihood - COMPLEXITY*networkLength;
			  break;
			case 2:
			  // LogLikelihood
			  sum_Likelihood = (ACCURACY )*10*sum_Likelihood - COMPLEXITY*networkLength;
			  break;
		}
     
		//Score of the network
	    networkScore = -1/(sum_Likelihood + 0.00000001);

	}
	else if (SCORE_TYPE == 1) //bayes
	{
		sum_Likelihood = (ACCURACY )*.1*sum_Likelihood - COMPLEXITY*networkLength;
		networkScore = sum_Likelihood ;
	}
	else if (SCORE_TYPE == 2) //Herskovits
	{
		networkScore = sum_Likelihood ;
	}
	tmp_NetworkLength=networkLength;
	tmp_paramLength=paramLength;
	tmp_NetworkScore=networkScore;
	tmp_BestFitness=(1.0/tmp_NetworkScore);
	return networkScore;

}




CMatrix C1AgentDoc::CreateNodeProbability(string nodeName) 
{
	//This calculates node probabilities using statistical frequency ( Can be used for Drichlet !!)
	//assumes node has no parents
	//i is now zero based node numberr
	float   value;
	float f_value;

	CNode* pNode=GetNode(nodeName);
	int l=pNode->m_NumOfStates;
	CCaseRecordItem tmpItem;
	tmpItem.NodeName=pNode->m_NodeName;
	CCaseRecordItem* tmpItem_returned;

	int		a = m_CaseRecords.CaseCount();

	CMatrix   nodeProb(l,1), nodeProbNum(l,1);

	for (int p=0; p < l; p++)
	{
		tmpItem.Value=(float)p; //which state we want the probability of
		
		tmpItem_returned=m_CaseRecords.GetRecordItem(tmpItem);

		if (tmpItem_returned==0)
		{
			value=0.0;
		}
		else
		{
			value=(float)tmpItem_returned->Occurrences;
		}

		nodeProbNum.SetElement(p,0,value); //occurrences
		
		if (a==0)
		{
			f_value = 1.0f/float(l);
		}
		else
		{
			f_value = value/float(a);
		}
		nodeProb.SetElement(p,0,f_value); //values

	}

	pNode->m_NodeCPTnum = nodeProbNum;

	//check for zeros in all elements of m_Prob
	//this happen when no case data to do calculation
	bool allZeros=true;
	for(int i=0;i<pNode->m_NumOfStates;i++)
	{
		if(nodeProb.GetElement(i,0)!=0.0)
		{
			allZeros=false;
			break;
		}
	}
	if(allZeros)
	{
		for(int i=0;i<pNode->m_NumOfStates;i++)
		{
			nodeProb.SetElement(i,0,1.0f/(float)pNode->m_NumOfStates);
		}
	}


	return nodeProb;
}

void C1AgentDoc::CalFirstLevelProbs()
{

	int l = m_oaNodes.size();
	
	//Create m_NodeCPTdenum for the first level variables
	CMatrix denCPT(1,1);
	denCPT.SetElement(0,0,float(m_CaseRecords.CaseCount()));

	for (int i=0; i < l; i++)
	{
		if (GetNode(i)->s_Parent.ItemCount() == 0) //IF NO PARENTS 
		{
			GetNode(i)->m_Prob = CreateNodeProbability(GetNode(i)->m_NodeName);
			GetNode(i)->m_NodeCPT = GetNode(i)->m_Prob;
			GetNode(i)->m_NodeCPTdenum = denCPT;
		}
	}
}


void C1AgentDoc::SetEvidence(string NodeName, int state)
{
	CNode *pNode = GetNode(NodeName);

	//check to see if node exists
	//non x-y agents could return bad values when moved in the 2-d grid
	//**********************************************************************
	if (pNode == 0) //state is zero based and m_MAX_STATES
		//is a count, (counts are usually one more than 0 based indices
	{
		return; //no nodes with that name
	}
	//**********************************************************************
	//**********************************************************************
	
	//check bounding value for passed node
	//**********************************************************************
	if (state >= pNode->m_MAX_STATES) //state is zero based and m_MAX_STATES
		//is a count, (counts are usually one more than 0 based indices
	{
		return; //no nodes allowed with more than m_MAX_STATES states
	}
	//**********************************************************************
	//**********************************************************************

	//next check to see if state is outside our available states
	//for this variable (like exploration)
	//**********************************************************************
	if (state>=pNode->m_NumOfStates)
	{//need to grow this node
		pNode->m_Prob.AddRow(state - pNode->m_NumOfStates + 1);	
		pNode->m_NumOfStates=state+1;
		//UpdateNodeCPT_AllNodes();
		CreateCPT(pNode->m_NodeName);
	}

	//**********************************************************************
	//**********************************************************************


	//set all state probabilities to zero for this node
	//this is initialization, the correct state
	//will be set to one later
	//********************************************************************
	for (int i=0; i < pNode->m_Prob.m_row; i++)
	{
		pNode->m_Prob.SetElement(i,0,0);
	}
	//**********************************************************************
	//**********************************************************************
	
	pNode->m_EvidenceFlag = -2;				//"-2" shows that this is the node that evidence enters
	pNode->m_Prob.SetElement(state,0,1);
	pNode->m_InstantiatedState = state;


	string xNodeName="X_" + m_AgentName;
	string yNodeName="Y_" + m_AgentName;
	
	int numStates=pNode->m_NumOfStates;
	


	Inference(NodeName);

}


void C1AgentDoc::OnRenewOrUpdateNetwork()
{

	//This function creates the networks for the first time after a database is loaded
	// Or renews and prepare a network that is created manually after a training has been done it.

	
	CalFirstLevelProbs();

}



double C1AgentDoc::CalculateLikelihood(CNode& rNode, CCaseRecord *passRecord)
{
	//passRecord is a pointer to actual case record
	//need to construct correct segment
	float retCPT;
	//CNode* pNode=GetNode(nodename);
	string nodename=rNode.m_NodeName;
	CNode& pNode=rNode;
	int state;
	int parentCount=pNode.s_Parent.ItemCount();
	if (!(passRecord->ContainsParentsAndNode(nodename,pNode.s_Parent)))
	{
		return 1.0f/(float)pNode.m_NumOfStates;
	}
	if (parentCount==0)
	{
		CCaseRecordItem* pItem=passRecord->GetItem(nodename);
		//zero level prob (m_prob)
		retCPT=(float)pItem->Occurrences/(float)m_CaseRecords.CaseCount();
		//TRACE(_T("parentless cpt node state  %f %s %d parents= %d \n"),retCPT ,nodename,(int)pItem->Value,parentCount);
		return retCPT;
	}
	CCaseRecord* getSegment=pNode.GetCaseRecord_Template();
	char buffer[1];
	std::list<CCaseRecordItem*>::iterator ip;
	for (ip=getSegment->ItemList.begin();ip!=getSegment->ItemList.end();ip++)
	{
		CCaseRecordItem* gI=(*ip);
		CCaseRecordItem* rI=passRecord->GetItem(gI->NodeName);
		gI->Value=rI->Value;
		if (gI->NodeName==(string)nodename)
		{
			state=(int)(gI->Value);
		}
		else //its a parent, so make part of trace string
		{
			int q=(int)(gI->Value);
		}
		gI->Occurrences=rI->Occurrences; //not really needed
	}
	
	
	retCPT= GetCPT_from_Matrix(nodename,state,getSegment);
	return retCPT;

}



float C1AgentDoc::GetCPT(string &NodeName, int state,CCaseRecord* pR)
{
	//assume that pR is already a constructed segment
	//including the node item

	//CAVEAT do not pass a referecnce to an actual
	//permutation segment or this will not work

	CCaseRecord* segment;
	float num,denom;
	num=0.0f;denom=0.0f;
	CNode* pNode=GetNode(NodeName);

	if(pNode->s_Parent.ItemCount()==0)
	{
		CCaseRecordItem tmpItem;
		tmpItem.NodeName=NodeName;
		tmpItem.Value=(float)state;
		CCaseRecordItem* pItem=m_CaseRecords.GetRecordItem(tmpItem);
		//zero level prob (m_prob)
		if (pItem==0)
		{
			return 0.0f;
		}
		else
		{
			return (float)pItem->Occurrences/(float)m_CaseRecords.CaseCount();	
		}
	}

	CCaseRecordItem* gItem;
	gItem=pR->GetItem(NodeName);
	float holdState=gItem->Value;
	
	for (int i=0;i<pNode->m_NumOfStates;i++)
	{
		gItem->Value=(float)i;
		segment=m_PermutationSegments.GetRecord(pR);
		if (segment!=0)
		{
			denom=denom+(float)segment->m_Occurrences;
			if(i==state)
			{
				num=(float)segment->m_Occurrences;
			}
		}
	}
	gItem->Value=holdState;
	if (denom==0.0f)
	{
		return 1.0f/(float)(pNode->m_NumOfStates);
	}
	else
	{
		return num/denom;
	}
}

void C1AgentDoc::LookForNewSegmentRecords()
{
	std::list<CCaseRecord*>::iterator ip42;
	for(ip42=m_CaseRecords.RecordList.begin();  ip42!=m_CaseRecords.RecordList.end();ip42++ )
	//for (int i=0; i< m_CaseRecords.RecordCount();i++)
	{
		CCaseRecord* pR=*ip42;
		m_PermutationSegments.AddRecord(pR,&m_oaNodes,false);
	}
	
	for(ip42=m_PermutationSegments.RecordList.begin();  ip42!=m_PermutationSegments.RecordList.end();ip42++ )
	//for ( i=0;i<m_PermutationSegments.RecordCount();i++)
	{
		CCaseRecord* aR=*ip42;
		aR->IsNew=false;
	}
}

int C1AgentDoc::NodeLength(string nodeName)
{
	//NodeLength divided by m_NumStates is the number of permutations of parents
	CNode* pNode=GetNode(nodeName);
	int m=pNode->m_NumOfStates-1;
	for (int i=0;i<pNode->s_Parent.ItemCount();i++)
	{
		CNode* gNode=GetNode(*pNode->s_Parent[i]);
		m=m*gNode->m_NumOfStates;
	}
	return m;
}

float C1AgentDoc::BayesScore(CNode *pNode)
{
	//Local Variables
	int     row = pNode->m_NumOfStates; //old cpt rows
	int     col = NodeLength(pNode->m_NodeName)/row; //old cpt matrix columns
	//above is number of permutations of parent states
	int     factorial;
	double   n_posterior = 1.0, d_posterior = 0.0;
	double    prior, d_prior = 1.0, posterior = 0.0;

	//Calculation of the priors
	for (int i=0; i < row; i++)
	{
		d_prior = 2*d_prior; //Gamma(3) = 2! = 2;
	}

	prior = Gamma(3*row)/d_prior;

	//Calculation of the posteriors
	//loop over parent permutations  we're bascially looping over cpt table
	//in new implementation, i will loop over segment data
	std::list<CCaseRecord*>::iterator ip43;
	for(ip43=m_PermutationSegments.RecordList.begin();  ip43!=m_PermutationSegments.RecordList.end();ip43++ )
	{

		CCaseRecord* pRecord=*ip43;
		CCaseRecordItem* pItem=pRecord->GetItem(pNode->m_NodeName);
		if( pRecord->ContainsParentsAndNode(pNode->m_NodeName,pNode->s_Parent) )
		{
			d_posterior = (double)pRecord->m_Occurrences/GetCPT(pNode->m_NodeName,(int)(pItem->Value),pRecord) + double(3*row) - 1.0;

			factorial = int(pRecord->m_Occurrences) + 2;
			
			posterior = log(prior) + posterior;

			for(int fact = factorial; fact > 0; fact--)
			{
				n_posterior = double(fact);
				posterior = posterior + log(n_posterior/d_posterior);
				d_posterior--;
			}

		}
		
	}


	return  float(posterior);


}
long double C1AgentDoc::Gamma(unsigned int i)
{

	i--;
	//Local variables
	long double  tao2 = 1.0, m;

	//Stirling's approximation for caluculating factorials.
//	tao1 = sqrt(2*PI*i)*exp(-i)*pow(i,i);

	//Calculation of a factorial using loops.
	if (i > 0)
	{
		for (int k=1; k < i + 1;k++)
		{
			m = (long double)(k);
			tao2 = tao2*m;
		}
	}
	
	return tao2;


}

bool C1AgentDoc::IsNetworkCyclic_new( list<CArrow> *arrows)
{
	std::list<CArrow>::iterator ip3;
	int IncludedNodes=0;
	//turn on all arrows
	std::list<CArrow>::iterator ip;
	for ( ip = m_oaArrows.begin(); ip != m_oaArrows.end(); ip++ ) 
	//for (int jj=0; jj< arrows->size();jj++)
	{
		//CArrow* gArrow=(CArrow*)arrows->GetAt(jj);
		(*ip).boolTag=true;//include in consideration
	}
	//first time through makes sure that node booltags are intialized
	//properly and it gets rid of all parentless nodes and arrows
	std::list<CNode>::iterator ip2;
	for ( ip2 = m_oaNodes.begin(); ip2 != m_oaNodes.end(); ip2++ ) 
	//for (int i=0; i< m_oaNodes.size();i++)
	{
		CNode* gNode=&(*ip2);
		if (gNode->s_Parent.ItemCount()==0)
		{
			gNode->boolTag=false; //remove from consideration
			//and remove arrows emanating from this node
			//----------------------------------------------
			for ( ip3 = m_oaArrows.begin(); ip3 != m_oaArrows.end(); ip3++ ) 
			//for (int j=0; j< arrows->size();j++)
			{
				//CArrow* gArrow=(CArrow*)arrows->GetAt(j);
				CArrow* gArrow=&(*ip3);
				if (gArrow->s_HeadNodeName==gNode->m_NodeName || gArrow->s_TailNodeName==gNode->m_NodeName)
				{
					gArrow->boolTag=false;//remove from consideration
				}
			}
			//----------------------------------------------
		}
		else
		{
			gNode->boolTag=true; //enable for consideration
			IncludedNodes+=1;
		}
	}
	if (IncludedNodes==0)//no more nodes in the network
	{
		return false; //not a cyclic network
	}
	//88888888888888888888888888888888888888888888888888888888888888888888
	int holdIncludedNodes=IncludedNodes+1;
	while (holdIncludedNodes > IncludedNodes )
	{	
		holdIncludedNodes=IncludedNodes;
		for ( ip2 = m_oaNodes.begin(); ip2 != m_oaNodes.end(); ip2++ ) 
		//for (int i=0; i< m_oaNodes.size();i++)
		{
			CNode* gNode=&(*ip2);
			if (gNode->boolTag) //if node hasn't been removed yet
			{
				bool ArrowsComingIn=false;
				//look for arrows(that have not been removed yet) with heads on this node
				//----------------------------------------------
				
				for ( ip3 = m_oaArrows.begin(); ip3 != m_oaArrows.end(); ip3++ ) 
				//for (int j=0; j< arrows->size();j++)
				{
					CArrow* gArrow=&(*ip3);
					if (gArrow->boolTag && gArrow->s_HeadNodeName==gNode->m_NodeName )
					{
						ArrowsComingIn=true;//node has unremoved arrow head
						break;
					}
				}
				if (!ArrowsComingIn)
				{
					gNode->boolTag=false; //remove from consideration
					IncludedNodes-=1;
					//and remove arrows emanating from this node
					//----------------------------------------------
					for ( ip3 = m_oaArrows.begin(); ip3 != m_oaArrows.end(); ip3++ ) 
					//for (int i=0; i< arrows->size();i++)
					{
						CArrow* gArrow=&(*ip3);
						if (gArrow->s_HeadNodeName==gNode->m_NodeName || gArrow->s_TailNodeName==gNode->m_NodeName)
						{
							gArrow->boolTag=false;//remove from consideration
						}
					}
				}
				//----------------------------------------------
			}
		}//end loop over nodes
		
	}//end while

	if(IncludedNodes==0)
	{	
		return false;
	}
	else
	{
		return true;
	}	
}

CMatrix C1AgentDoc::GetCPTColumn(string &NodeName,CCaseRecord* pR,float& column_total)
{
	//assume that pR is already a constructed segment
	//including the node item

	//CAVEAT do not pass a referecnce to an actual
	//permutation segment or this will not work

	CCaseRecord* segment;
	float num,denom;
	num=0.0f;denom=0.0f;
	CNode* pNode=GetNode(NodeName);

	//define matrix to return values
	CMatrix tmpMatrix(pNode->m_NumOfStates,1);
	//constructed matrix is already equal probability weighted

	CCaseRecordItem* gItem;
	gItem=pR->GetItem(NodeName);
	float holdState=gItem->Value;
	
	for (int i=0;i<pNode->m_NumOfStates;i++)
	{
		gItem->Value=(float)i;
		segment=m_PermutationSegments.GetRecord(pR);
		if (segment!=0)
		{
			denom=denom+(float)segment->m_Occurrences;
			//if(i==state)
			//{
				//num=(float)segment->m_Occurrences;
				tmpMatrix.SetElement(i,0,(float)segment->m_Occurrences);
			//}
		}
		else
		{
			tmpMatrix.SetElement(i,0,0.0f);
		}
	}
	gItem->Value=holdState;
	if (denom!=0.0f)
	{
		//return num/denom;
		for ( int i=0;i<pNode->m_NumOfStates;i++)
		{
			float element=tmpMatrix.GetElement(i,0);
			tmpMatrix.SetElement(i,0,element/denom); //convert numbers to fractions
		}
	}
	else //matrix may be all zeros so we have to equal weight it agian
	{
		tmpMatrix.Equalize();
	}
	column_total=denom; //return column total for filling number cpt arrays
	return tmpMatrix;

}

int C1AgentDoc::ParentPermutations(string &nodeName)
{
	int perms=1;
	CNode* pNode=GetNode(nodeName);
	// printf("Parent Count..%d \n\n", pNode->s_Parent.ItemCount());
	for (int i=0; i<pNode->s_Parent.ItemCount();i++)
	{
		string* tmpName=pNode->s_Parent[i];
		CNode* parent=GetNode(*tmpName);
		// printf("Parent %d has %d states..\n\n", i+1, parent->m_NumOfStates);
		perms=perms*parent->m_NumOfStates;
	}
	return perms;
}


float C1AgentDoc::GetCPT_from_Matrix(string &NodeName, int state,CCaseRecord* pR)
{
	//assume that pR is already a constructed segment
	//including the node item

	//CAVEAT do not pass a referecnce to an actual
	//permutation segment or this will not work

	float num,denom;
	num=0.0f;denom=0.0f;
	CNode* pNode=GetNode(NodeName);

	//NO parents for this node
	if(pNode->s_Parent.ItemCount()==0)
	{
		CCaseRecordItem tmpItem;
		tmpItem.NodeName=NodeName;
		tmpItem.Value=(float)state;
		CCaseRecordItem* pItem=m_CaseRecords.GetRecordItem(tmpItem);
		//zero level prob (m_prob)
		if (pItem==0)
		{
			return 0.0f;
		}
		else
		{
			return (float)pItem->Occurrences/(float)m_CaseRecords.CaseCount();	
		}
	}



	CCaseRecordItem* pI= pR->GetItem(pNode->m_NodeName);
	//pI is the recordItem of the child node in the template
	//column calculation (column of cpt table)
	//same as row of supermatrix
	int m=1;
	int column=0; 
	for (int q=pNode->s_Parent.ItemCount()-1;q>-1;q--)
	{
		string passName=*(pNode->s_Parent[q]);
		CNode* qNode=GetNode(passName);
		CCaseRecordItem* pI2=pR->GetItem(qNode->m_NodeName);
		column=column+(int)(pI2->Value)*m;
		m=m*qNode->m_NumOfStates;
	}


	return pNode->m_NodeCPT.GetElement(state,column);
}

CNode* C1AgentDoc::GetNode(string NodeName)
{
	
	std::list<CNode>::iterator ip;
	for ( ip = m_oaNodes.begin(); ip != m_oaNodes.end(); ip++ ) 
	{
		CNode& rNode=(*ip);
		if ( (rNode.m_NodeName)==NodeName)
		{
			return &(*ip);
		}
	}
	return 0;

}



void C1AgentDoc::ClearHeapObjects()
{
	//zero out nodes and arrows and decoder and RecordList
	//***************************************************

	m_oaNodes.clear();
	m_oaArrows.clear();


	std::list<CCaseRecord*>::iterator ip42;
	while (!m_CaseRecords.RecordList.empty())
	{
		ip42=m_CaseRecords.RecordList.begin();
		CCaseRecord* tmpRecord=*ip42;
		delete tmpRecord;
		m_CaseRecords.RecordList.pop_front();
	}
	m_CaseRecords.RecordList.clear();
	while (!m_PermutationSegments.RecordList.empty())
	{
		ip42=m_PermutationSegments.RecordList.begin();
		CCaseRecord* tmpRecord=*ip42;
		delete tmpRecord;
		m_PermutationSegments.RecordList.pop_front();
	}
	m_PermutationSegments.RecordList.clear();


	std::list<CCaseRecordItem*>::iterator ip;
	while (!m_CaseRecords.ItemList.empty())
	{
		ip=m_CaseRecords.ItemList.begin();
		CCaseRecordItem* tmpItem=*ip;
		delete tmpItem;
		m_CaseRecords.tmp_delete_CCaseRecordItems+=1;
		m_CaseRecords.ItemList.pop_front();
	}
	m_CaseRecords.ItemList.clear();


	while (!m_PermutationSegments.ItemList.empty())
	{
		ip=m_PermutationSegments.ItemList.begin();
		CCaseRecordItem* tmpItem=*ip;
		delete tmpItem;
		m_PermutationSegments.ItemList.pop_front();
	}
	m_PermutationSegments.ItemList.clear();
	//---------
	//***************************************************
	//***************************************************

}

////void C1AgentDoc::ReadNodesFrom_File(char* node_sizes_filename,char* node_names_filename)
void C1AgentDoc::ReadNodesFrom_File(char * FileName )
{

	int incNode=1,addInRow=0,j=0,NodeNameSize;
	
////
#define unsigned const int MaxNumOfCol=256,MaxColLength=16;
    unsigned int  count,i, k, NumOfCol;
	//unsigned const int MaxNumOfCol=256,MaxColLength=16;
    int c;
	int NodeSizes[MaxNumOfCol]; 
    char head[MaxNumOfCol][MaxColLength];
	
	/********/

    FILE* g=fopen(FileName,"rt");
    c = getc( g ) ; 
    count = (c & 0x00ff)  << 24;
    c = getc( g ) ; 
    count = count | ((c & 0x00ff)  << 16);
    c = getc( g ) ; 
    count = count | ((c & 0x00ff)  << 8);
    c = getc( g ) ; 
    count =  count | (c & 0x00ff)  ;

    printf("count = %d \n",count) ; 

    if ( ( c = getc( g ) ) < 0 ) {
         printf("Error .. No Column information \n");
         exit(1);
          } 
      else 
          NumOfCol= c; 
          printf("NumOfCol = %d \n", NumOfCol);

	   
		  for (i=0 ; i < NumOfCol; i++){
					c = getc( g )  ;
			NodeSizes[i]=((int) c) -47; 
            printf("NodeSizes[%d] = %d \n", i,NodeSizes[i]);
		  }
		  printf("\n");
		
	   k=0;
       for (i=0 ; i < NumOfCol; i++) { 
                while ( (( c = getc( g ) ) != 32) &&(c!='\n') ) head[i][k++]=c; 
                head[i][k]= '\0'; 
                k=0;
           }
       for (i=0 ; i < NumOfCol; i++)  
             printf("%s \n", head[i]);

      //k= count/2 ;
       //      printf("%d \n", k);

////
	char NodeSizesFile[255];// For nodesizes_*.dat
	char NodeNamesFile[255];// For nodenames_*.dat
	sprintf(NodeSizesFile,"nodesizes_%s",FileName);
	sprintf(NodeNamesFile,"nodenames_%s",FileName);
	   
	FILE* n_sizes=fopen(NodeSizesFile,"w");
	FILE* n_names=fopen(NodeNamesFile,"w");
	///// cet  FILE* f=fopen(node_sizes_filename,"rt");
      //  FILE* g=fopen(node_names_filename,"rt");
	char NodeName[25];//buffer
	printf("reading nodename from %s\n",FileName);
	////printf("reading nodename from %s\n",node_names_filename);
	//while(EOF!=fscanf(g,"%s\n",NodeName))


	for (i=0 ; i < NumOfCol; i++)  
	{
		strcpy(NodeName, head[i]);
        
		fprintf(n_names,"%s\n", head[i]);
		fprintf(n_sizes,"%d\n", NodeSizes[i]);
		//fscanf(f,"%d\n",&NodeNameSize);
		
		NodeNameSize=NodeSizes[i];

		string strNodeName;
	//	string tmpStr=NodeName;

		strNodeName = NodeName;
	//	strNodeName.append(tmpStr,0,NodeNameSize);
	//	strcat(tmpStr, NodeNameSize);

	//	printf("The node name is %s  :   \n",strNodeName.c_str());

		CNode& rNode=AddNode();
		rNode.m_NodeName=strNodeName;
		//node number 1 based necessary evil for cpt calcualtions for now
		rNode.m_NodeNumber = j + 1;

	}

	fclose(g);//fclose(f);
	fclose(n_names);
	fclose(n_sizes);

}

void C1AgentDoc::ReadCasesFrom_File(char * FileName)
{
/////
//	char FileName[]="simpletest.txt";
	//#define unsigned const int MaxNumOfCol=256,MaxColLength=16;
	unsigned int  count=0,i, j, NumOfCol,b[2]; //,b1, b2;
	int c,ItemVal,k; 
	char head[MaxNumOfCol][MaxColLength];
	char WriteFile[255]; // For cases_*.dat

	sprintf(WriteFile,"cases_%s",FileName);
	
	FILE* h=fopen(FileName,"rt");
	FILE* cases=fopen(WriteFile,"w");	

    c = getc( h ) ; 
    count = (c & 0x00ff)  << 24;
    c = getc( h ) ; 
    count = count | ((c & 0x00ff)  << 16);
    c = getc( h ) ; 
    count = count | ((c & 0x00ff)  << 8);
    c = getc( h ) ; 
    count =  count | (c & 0x00ff)  ;

    printf("count = %d \n",count) ; 

    if ( ( c = getc( h ) ) < 0 ) {
         printf("Error .. No Column information \n");
         exit(1);
          } 
      else 
          NumOfCol= c; 
          printf("NumOfCol = %d \n", NumOfCol);
		
		
	    for (i=0 ; i < NumOfCol; i++)
                ( ( c = getc( h ) ) ); //NodeSizes[i]=c; moving the cursor forward to bypass the node sizes

       j=0;
       for (i=0 ; i < NumOfCol; i++) { 
                while ( ( c = getc( h ) ) != 32 ) head[i][j++]=c; 
                head[i][j]= '\0'; 
               j=0;
           }
       //for (i=0 ; i < NumOfCol; i++)  
       //      printf("%s \n", head[i]);


	   	j= count/2 ;
             printf("%d \n", j);

	   // if the number of elements is odd we ignore the last line in the data
	if ( count %2 == 1) {
         count=count-NumOfCol; 
		 //  if ( (c = getc( stdin )) >= 0 )  {
         //      b1 = (c & 0x000f) +48;
         //      b2 = ((c >> 4) & 0x000f) +48;
         //      putc ((unsigned char) b1, stdout);
         //    } 
		 j = count/2;

		 printf("%d \n", j);
	}
	

/* for (i=0 ; i < (count)/2 ;i++){
           if ( (c = getc( h )) >= 0 )  {
               b1 = (c & 0x000f) +48;
               b2 = ((c >> 4) & 0x000f) +48;
               putc ((unsigned char) b2, stdout);
               putc ((unsigned char) b1, stdout);
             }
         else {
             printf("ERROR in count i=%d \n", i);
             exit(0);
            }
      } //for
       // Take care the last element if count is ODD
        if ( count %2 == 1) {
           if ( (c = getc( h )) >= 0 )  {
               b1 = (c & 0x000f) +48;
              // b2 = ((c >> 4) & 0x000f) +48;
               putc ((unsigned char) b1, stdout);
             }
         }
*/

//////
// 	FILE* h=fopen("cases.dat","rt");
	//read formatted output file of integers cases.dat
	int iCount=0;//need to add a record after
	//we count to the sum of the nodes in the collection
	int NODES=m_oaNodes.size();

	//printf("number of node  %d\n", NODES);

	std::list<CNode>::iterator ip;
	ip=m_oaNodes.begin();
	CCaseRecord* addRecord;
	float ItemValue;
	//while (0!=fread(&ItemValue,sizeof(float),1,h))
	//for (int i=0; i< m_CaseRecords.RecordCount();i++)
	int icount_1=0;
	////while(EOF!=fscanf(h,"%f\n",&ItemValue))



i=0;
		 //for (i=1; i<(count)/2 ;i++)
do 
{
		if ( (c = getc( h )) >= 0 )  
			{
             b[0] = (c & 0x000f) ;
             b[1] = ((c >> 4) & 0x000f);
           //  putc ((unsigned char) b2, stdout);
           //  putc ((unsigned char) b1, stdout);
             }
        else {
             printf("ERROR in count i=%d \n", i);
             exit(0);
			}

		for (k=0;k<2;k++)
		{
			ItemVal=b[k];
			ItemValue=float(ItemVal);

			fprintf(cases,"%f\n", ItemValue);

		//printf("reading cases %d\n",icount_1++);	
			if (ip==m_oaNodes.begin())
			{
			addRecord=new CCaseRecord();
			}
			CNode* pNode=&(*ip);
			if (!(ItemValue== 3200.0))
			{	
			CCaseRecordItem* addRecordItem= new CCaseRecordItem();
			addRecordItem->NodeName=pNode->m_NodeName;
			addRecordItem->Value=ItemValue;
			addRecord->ItemList.push_back(addRecordItem);
			}
			ip++;
			if(ip==m_oaNodes.end())
			{
				if (!m_CaseRecords.AddRecord(addRecord))
					{ //if we can't add the record, because
					//it has already been added then dump this one
	
					//first delete CCaseRecordItem Objects
					//**************************************
					std::list<CCaseRecordItem*>::iterator ip;
					while (!addRecord->ItemList.empty())
					//for (ip=addRecord->ItemList.begin();ip!=addRecord->ItemList.end();ip++)
					//for (int qrs=0; qrs< addRecord->ItemList.GetSize(); qrs++)
						{
						ip=addRecord->ItemList.begin();
						CCaseRecordItem* tmpItem=*ip;
						addRecord->ItemList.pop_front();
						delete tmpItem;
						m_CaseRecords.tmp_delete_CCaseRecordItems+=1;
						}
					addRecord->ItemList.clear();
					//**************************************
					//**************************************
					//now delete the object
					delete addRecord;
					//CString woohoo=tmpItem->NodeName;
					//int yyyuu=90;
					}
				ip=m_oaNodes.begin();
			}
		}

	i++;

} while (i<count/2);

printf("Number of Elements read %d\n", i);
		
fclose(h);fclose(cases);

}


void C1AgentDoc::ImprintNetworkFromPSO_particle(PSO_particle *pParticle)
{
	//loop over PlusN particle to add arrows to agent
	CNode* childNode;
	CNode* parentNode;
	for (int i=0; i<m_oaNodes.size();i++)
	{
		for (int j=0;j<m_oaNodes.size() ;j++)
		{
			if (pParticle->X_PlusN[(m_oaNodes.size())*i+j] && i!=j) //is a parent is indicated and not a parent of itself (cant happen butr check anyway)
			{
				childNode=GetNode(i);
				parentNode=GetNode(j);
				AddArrow(parentNode->m_NodeName,childNode->m_NodeName,true);//softadd
			}
		}
		
	}
	//done soft adding all the arrows

}

void C1AgentDoc::PrintNetworkToFile(char *ArrowFile,PSO_particle *pPartcl)
{   using std::ofstream;
	using std::string;
   	int i,j;
	CNode* childNode;
	CNode* parentNode;
	string p;
	string c;

	ofstream fout;
    fout.open(ArrowFile);

    if (fout.good()) {
		fout <<"digraph Arrows { size=\"7,7\" " ; fout <<"\n";

		for (i=0; i<m_oaNodes.size();i++) {
			for (j=0;j<m_oaNodes.size() ;j++) {
				if (pPartcl->X_PlusN[(m_oaNodes.size())*i+j] && i!=j)  { //is a parent is indicated and not a parent of itself (cant happen butr check anyway)
					childNode=GetNode(i);
					parentNode=GetNode(j);
					p = (parentNode->m_NodeName);
					c = (childNode->m_NodeName);
					//fout2 << ((m_oaNodes.size())*i+j); fout2 <<"\n";
					fout << p << "->"<< c ; fout <<"\n";
					//fprintf(fout, "%s  -> %s  \n", p.c_str(),c.c_str());
				}
			}
		}

		fout << "}";fout <<"\n";
	}
	else {
	printf("Cannot open %s : \n",ArrowFile );
	exit(1);
	}

fout.close( );
//done soft adding all the arrows
// fclose(fout);

}



double C1AgentDoc::Herskovits(CNode *pNode)
{
	double PI=3.1415926535898;
	//use exclusively the CPT table for this calculation
	double retVal=0;
	double r=(double)(pNode->m_NumOfStates);
	CMatrix& rM=pNode->m_NodeCPTnum;
	//loop over columns of m_NodeCPTnum
	for(int col=0;col<rM.m_col;col++)
	{
		double Nij=0;
		double Nijk=0;
		//loop over rows
		for(int row=0;row<rM.m_row;row++)
		{
			if(rM(row,col)!=0.0)
			{
				//Nijk=Nijk +  rM(row,col)*log(rM(row,col))-rM(row,col);
				Nijk=Nijk +  0.5*log10(2.0*PI*rM(row,col)+PI/3.0)   + rM(row,col)*log10(rM(row,col))  - rM(row,col)*log10(exp(1.0));
			}
			Nij=Nij + rM(row,col);	
		}
		//retVal=retVal+(Nijk)+ ((r-1)*log(r-1)- (r-1) )   -   (   (Nij+r-1) * log(Nij+r-1) -  (Nij+r-1) );
		double termNum=0.5*log10(2.0*PI*(r-1)+PI/3.0)  + (r-1)*log10((r-1))  -  (r-1)*log10(exp(1.0));
		double termDenom=0.5*log10(2.0*PI*(Nij+r-1)+PI/3.0)  + (Nij+r-1)*log10((Nij+r-1))  -  (Nij+r-1)*log10(exp(1.0));
		retVal=retVal+(Nijk)  + termNum   -  termDenom;
	}
	return retVal;	
}

void C1AgentDoc::ClearPermutations()
{

        std::list<CCaseRecord*>::iterator ip42;
        while (!m_PermutationSegments.RecordList.empty())
        {
                ip42=m_PermutationSegments.RecordList.begin();
                CCaseRecord* tmpRecord=*ip42;
                delete tmpRecord;
                m_PermutationSegments.RecordList.pop_front();
        }
        m_PermutationSegments.RecordList.clear();

	std::list<CCaseRecordItem*>::iterator ip;
        while (!m_PermutationSegments.ItemList.empty())
        {
                ip=m_PermutationSegments.ItemList.begin();
                CCaseRecordItem* tmpItem=*ip;
                delete tmpItem;
                m_PermutationSegments.ItemList.pop_front();
        }
        m_PermutationSegments.ItemList.clear();
        //---------
        //***************************************************
}
void C1AgentDoc::UpdateNetwork()
{
	int nodeCount = m_oaNodes.size();       // The number of nodes in the network
	int upt=0;

	CalFirstLevelProbs(); //JCT mod
	
	for (int y=0; y < nodeCount; y++)
	{
		if (GetNode(y)->m_Modified == false) 
		{
			CNode *pCurNode = GetNode(y);
			int	n_sCurParent = pCurNode->s_Parent.ItemCount();
			if (n_sCurParent!= 0)
			{
				pCurNode->m_Prob.OnZero();
				//This portion updates the selected node
				CMatrix prntProb;

				prntProb = Parents(pCurNode->m_NodeName);
				pCurNode->m_Prob = (pCurNode->m_NodeCPT)*prntProb;
				//end new calculation
				pCurNode->m_Modified = true;
			}
			else
				pCurNode->m_Modified = true;    // This directly modifies the first level nodes 
		}
	}	


}
double C1AgentDoc::JasonSingleArcScore(CNode *pNode)
{
        //use exclusively the CPT table for this calculation
        double retVal=0;
        double Nijk=0;
        CMatrix& rM=pNode->m_NodeCPTnum;
        double r=(double)(pNode->m_NumOfStates);
        int passZero=0;
        CNode* parentNode=GetNode(*pNode->s_Parent[passZero]);//assumes 1 parent
        //loop over columns of CPT table
        double Joint=0;
        for(int col=0;col<rM.m_col;col++)
        {
                for(int row=0;row<rM.m_row;row++)
                {
                        if(parentNode->m_NodeCPTnum.GetElement(col,0) !=0.0){
                                CCaseRecord* tmpRecord=new CCaseRecord();
                                CCaseRecordItem* tmpItem1=new CCaseRecordItem();
                                tmpItem1->NodeName=pNode->m_NodeName;
                                tmpItem1->Value=(float)row;
                                tmpRecord->ItemList.push_back(tmpItem1);
                                CCaseRecordItem* tmpItem2=new CCaseRecordItem();
                                tmpItem2->NodeName=parentNode->m_NodeName;
                                tmpItem2->Value=(float)col;
                                tmpRecord->ItemList.push_back(tmpItem2);
                                CCaseRecord* getRecord=m_PermutationSegments.GetRecord(tmpRecord);
                                if (getRecord==0){
                                        Joint=0;
                                }
                                else{
                                        Joint=(double)getRecord->m_Occurrences/(double)m_CaseRecords.CaseCount();
                                        int woohey=90;
                                }
                                
                                //Nijk=Nijk +  rM(row,col)*log(rM(row,col))-rM(row,col);
                                double Joint2=( Joint / (GetCPT(parentNode->m_NodeName,col,tmpRecord)) ) ;
                                Nijk=Nijk + fabs( GetCPT(pNode->m_NodeName,row,tmpRecord) - Joint2);
                                int jason=42;
                                delete tmpItem1;delete tmpItem2;
                                delete tmpRecord;
                        }
                }
        }               
        retVal=1/Nijk;
		TwoColumnCrossCorrelation=m_CaseRecords.Correlation(pNode->m_NodeName,parentNode->m_NodeName);
        TwoColumnCrossCorrelation_Independent=m_CaseRecords.ExpectedValue(pNode->m_NodeName)*m_CaseRecords.ExpectedValue(parentNode->m_NodeName);

        TwoColumnCrossCovariance=TwoColumnCrossCorrelation-TwoColumnCrossCorrelation_Independent;
        CorrelationCoefficient=TwoColumnCrossCovariance/sqrt(m_CaseRecords.Variance(pNode->m_NodeName)*m_CaseRecords.Variance(parentNode->m_NodeName));

        return retVal;

}
