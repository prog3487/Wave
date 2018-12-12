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
		
		void CreateVertexBuffer(ID3D11Device* device)
		{
			CD3D11_BUFFER_DESC desc(ByteWidthVertices(), D3D11_BIND_VERTEX_BUFFER);
			D3D11_SUBRESOURCE_DATA vInitData = { 0 };
			vInitData.pSysMem = vertices.data();
			DX::ThrowIfFailed(
				device->CreateBuffer(&desc, &vInitData, VB.ReleaseAndGetAddressOf()));
		}

		void CreateIndexBuffer(ID3D11Device* device)
		{
			CD3D11_BUFFER_DESC desc(ByteWidthIndices(), D3D11_BIND_INDEX_BUFFER);
			D3D11_SUBRESOURCE_DATA vInitData = { 0 };
			vInitData.pSysMem = indices.data();
			DX::ThrowIfFailed(
				device->CreateBuffer(&desc, &vInitData, IB.ReleaseAndGetAddressOf()));
		}
		
		std::vector<T> vertices;
		std::vector<I> indices;

		Microsoft::WRL::ComPtr<ID3D11Buffer> VB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> IB;
	};
}