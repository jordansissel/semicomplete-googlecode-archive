#include "Arrow.h"
#include <math.h>

#define  PI  4*atan(1)


CArrow::CArrow()
{
	boolTag=false;
}

CArrow::~CArrow()
{

}

CArrow* CArrow::operator= (const CArrow &arrowToCopy) //copy assignment operator
{
	this->boolTag=arrowToCopy.boolTag;
	this->s_HeadNodeName=arrowToCopy.s_HeadNodeName;
	this->s_TailNodeName=arrowToCopy.s_TailNodeName;
	return this;
}
CArrow::CArrow(const CArrow &arrowToCopy) //copy constructor
{
	this->boolTag=arrowToCopy.boolTag;
	this->s_HeadNodeName=arrowToCopy.s_HeadNodeName;
	this->s_TailNodeName=arrowToCopy.s_TailNodeName;
}
bool CArrow::operator ==(const CArrow &arrow)
{
	if(this->boolTag==arrow.boolTag &&
	this->s_HeadNodeName==arrow.s_HeadNodeName	&&
	this->s_TailNodeName==arrow.s_TailNodeName)
	{
		return true;
	}
	else
	{
		return false;
	}
}
