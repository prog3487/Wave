#pragma once

#include "VertexTypes.h"
#include <vector>

namespace Bruce
{
	// T = vertex structure type
	// I = index type
	template <typename T, typename I = uint32_t>
	class Geometry
	{
	public:

		DXGI_FORMAT GetIndexDXGIFormat() const 
		{ 
			switch (sizeof(I))
			{
			case 2:	return DXGI_FORMAT_R16_UINT; break;
			case 4:	return DXGI_FORMAT_R32_UINT; break;
			}
			assert(!"can not find proper dxgi format. not implemented.");
			return DXGI_FORMAT_R32_UINT;
		}

		UINT Stride() const { return sizeof(T); }
		UINT ByteWidthVertices() const { return sizeof(T) * vertices.size(); }
		UINT ByteWidthIndices() const { return sizeof(I) * indices.size(); }
		
		std::vector<T> vertices;
		std::vector<I> indices;
	};
}