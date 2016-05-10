
/************************************************************

										.cpp:
				Point Collection, !!!REGARDLESS!! of order

************************************************************/

#include "MyConsoleEngine.h"

IPointCollection::IPointCollection()
{
	m_pVB_Mem = new std::vector<Vertex>;
	m_pIB_Mem = new std::vector<UINT>;
}

IPointCollection::~IPointCollection()
{
	delete m_pVB_Mem;
	delete m_pIB_Mem;
}

void IPointCollection::AddPoint(const Vertex & vec)
{
	m_pVB_Mem->push_back(vec);
	m_pIB_Mem->push_back(m_pVB_Mem->size()-1);
}

void IPointCollection::AddPoint(const VECTOR3 & pos, const VECTOR3 & color)
{
	Vertex v;
	v.pos = pos;
	v.color = { color.x,color.y,color.z,1.0f };
	v.normal = { 0,0,0 };
	v.texcoord = { 0,0 };
	m_pVB_Mem->push_back(v);
	m_pIB_Mem->push_back(m_pVB_Mem->size() - 1);
}

void IPointCollection::SetPoint(UINT index, const Vertex & vec)
{
	if (index < m_pVB_Mem->size())
	{
		m_pVB_Mem->at(index) = vec;
	}
}

void IPointCollection::SetPointPos(UINT index, const VECTOR3 & pos)
{
	if (index < m_pVB_Mem->size())
	{
		m_pVB_Mem->at(index).pos = pos;
	}
}

void IPointCollection::DeletePoint(UINT index)
{
	if (index < m_pVB_Mem->size())
	{
		std::swap(m_pVB_Mem->at(index), m_pVB_Mem->back());
		m_pVB_Mem->pop_back();
		m_pIB_Mem->pop_back();
	}
}

UINT IPointCollection::GetVertexCount()
{
	return m_pVB_Mem->size();
}

void IPointCollection::PopBack()
{
	if (m_pVB_Mem->size() > 0)m_pVB_Mem->pop_back();
}

void IPointCollection::ClearAll()
{
	m_pVB_Mem->clear();
	m_pIB_Mem->clear();
}
