
/***********************************************************************

									h£ºPointCollection

************************************************************************/

#pragma once

class IPointCollection
{
	public:
	
		friend class IRenderer;

		IPointCollection();

		IPointCollection(const IPointCollection& c) = delete;

		void		AddPoint(const Vertex& vec);

		void		SetPoint(UINT index,const Vertex& vec);

		void		SetPointPos(UINT index, const VECTOR3& pos);

		void		DeletePoint(UINT index);

		UINT		GetVertexCount();

		void		PopBack();

	private:
		std::vector<UINT>*		m_pIB_Mem;
		std::vector<Vertex>*		m_pVB_Mem;
};
