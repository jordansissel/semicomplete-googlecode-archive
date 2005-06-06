#include "Matrix.h"
#include <string>
using namespace std;
using std::string;


CMatrix::CMatrix()  // Default Constructor
{
	m_row = 2;
	m_col = 1;

	m_CPT.clear();

	//Fill the default values in the CPT.
	for (int i=0; i<2; i++)
		m_CPT.push_back(0.5);

}
CMatrix::CMatrix(const CMatrix & rhs)  //Copy constructor
{

	m_row = rhs.m_row;
	m_col = rhs.m_col;
	m_CPT.clear();
	
	for (int i = 0; i < m_row*m_col; ++i)
		m_CPT.push_back(rhs.m_CPT[i]);
}

CMatrix::CMatrix(int row, int col)   // Overloaded constructor
{
	m_CPT.clear();
	if (row > 0 && col >0)
	{

		m_row = row;
		m_col = col;
		float equalProb=1.0f/(float)m_row;

		for (int i = 0; i < row*col; ++i)
			m_CPT.push_back(equalProb);
	}
}

CMatrix::~CMatrix()
{

}

//Sets an element in a matrix.
void CMatrix::SetElement(int row, int col, float x)
{
	int p = row*m_col + col;
	m_CPT[p]=x;
}


float CMatrix::operator()(int i, int j)
{
	int number;

	number = this->m_col*i + j;


	return this->m_CPT[number];
}

float CMatrix::GetElement(int i, int j)
{
	return m_CPT[i*m_col + j];
	
}

				

CMatrix CMatrix::operator *(const CMatrix & rhs)
{

	if (m_col == rhs.m_row)
	{
		float s;
		//m_col = rhs.m_col;

		CMatrix  rMatrix(m_row, rhs.m_col);

		for (int i=0; i < m_row; i++)
			for (int j=0; j < rhs.m_col; j++)
			{
				s = 0.0;
				for (int k=0; k < m_col; k++)
				{
					int pass_int=i*m_col + k;
					int pass_int_2=k*rhs.m_col + j;
					s = s + (m_CPT[pass_int]*rhs.m_CPT[pass_int_2]);
				}
				
				rMatrix.SetElement(i, j, s);
			}



		return rMatrix;
	}
}

void CMatrix::operator =(const CMatrix &rhs)
{
	m_row = rhs.m_row;
	m_col = rhs.m_col;

	m_CPT.clear();

	for (int i = 0; i < m_row*m_col; i++)
		m_CPT.push_back(rhs.m_CPT[i]);

}


CMatrix CMatrix::Supermultiply(CMatrix &rhs)
{
	int l = m_row * rhs.m_row;
	float s;
	string st;

	CMatrix  SuperMatrix(l,1), B;

	B = rhs.Transpose();
	
	CMatrix  rMatrix(m_row, B.m_col);

	for (int i=0; i < m_row; i++)
		for (int j=0; j < B.m_col; j++)
		{
			s = 0.0;
			for (int k=0; k < m_col; k++)
			{
				s = s + (m_CPT[i*m_col + k]*B.m_CPT[k*B.m_col + j]);
			}
			rMatrix.SetElement(i, j, s);
		}

	int position = 0;

	for (int i=0; i < rMatrix.m_row; i++)
		for (int j=0; j < rMatrix.m_col; j++)
		{
			SuperMatrix.SetElement(position,0, rMatrix.GetElement(i,j));
			position++;
		}

	return SuperMatrix;
}

CMatrix CMatrix::Transpose()
{

	CMatrix	Transpose(m_col, m_row);

	for (int i=0; i < m_col; i++)
		for (int j=0; j < m_row; j++)
		{
			Transpose.SetElement(i,j,this->GetElement(j,i));
		}

	return	Transpose;

}


CMatrix::CMatrix(int row, int col, char I)
{

	if (I == 'I')
	{

		if (row > 0 && col >0)
		{

		m_row = row;
		m_col = col;

		for (int i = 0; i < row; i++)
			for (int j = 0; j < row; j++)
				if ( i==j)
					m_CPT.push_back(1);
				else
					m_CPT.push_back(0);
		}
	}

}

CMatrix CMatrix::NumOfStates()
{

	CMatrix retStates(1,this->m_col);
	

	for (int j=0; j < this->m_col; j++)
	{
		int maxState = 0;

		for (int i=0; i < this->m_row; i++)
		{
			if (this->GetElement(i,j) > maxState)
				maxState = this->GetElement(i,j);
		}
		retStates.SetElement(0,j, maxState + 1);

	}

	return retStates;
}


float CMatrix::CalculateJP(CMatrix &test, int m)
{

	//finds the nuumber times row m of matrix test
	//appears in the world data and divides by world data points
	//to arrive at joint probablility
	int counter;
	float p = 0.0;
	float prob;

	for (int i=0; i < this->m_row; i++)
	{
		counter = 0;

		for (int j=0; j < this->m_col; j++)
		{
			if (this->GetElement(i,j) == test.GetElement(m,j)) 
			{
				counter++;
			}
			
			if (counter == this->m_col)  
			{
				p++; 
			}

		}//end loop over columns
	}//end loop over rows

//	prob = p;
	prob = p /(this->m_row);

	return prob;


}

void CMatrix::AddRow(int i)
{

	this->m_row = this->m_row + i;

	for (int j=0; j < this->m_col*i; j++)
	{
		this->m_CPT.push_back(0);
	}
}


int CMatrix::MaxElement()
{
	//Local variables
	float  value = this->GetElement(0,0);
	float  dumy  = 0.0;
	int    index = -1;

	for (int i=0; i < this->m_row; i++)
	{
		dumy = this->GetElement(i,0);
		if (dumy > value)
		{
			value = dumy;
			index = i;
		}
	}

	if ( index == -1)
	{
		index = 0;
		return index;
	}
	else
		return index;

}

void CMatrix::OnZero()
{
	for (int i=0; i < m_row; i++)
	{
		for (int j=0; j < m_col; j++)
		{
			this->SetElement(i,j,0.0);
		}
	}

}

void CMatrix::Equalize()
{
	for (int i=0; i < m_row; i++)
	{
		for (int j=0; j < m_col; j++)
		{
			this->SetElement(i,j,1.0f/(float)m_row);
		}
	}
}

void CMatrix::ReInit(int &row, int &col) //1 based passed
{
	if (row > 0 && col >0)
	{
		m_CPT.clear();
		m_row = row;
		m_col = col;
		float equalProb=1.0f/(float)m_row;

		for (int i = 0; i < row*col; ++i)
		{
			m_CPT.push_back(equalProb);
		}
	}
	else
	{
		string str;
	}
}


