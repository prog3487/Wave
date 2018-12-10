#pragma once

#include <VertexTypes.h>

//
// Bruce VertexTypes...
//
namespace DirectX
{
	struct VertexPositionNormalTangentTexture
	{
		VertexPositionNormalTangentTexture() = default;

		VertexPositionNormalTangentTexture(const VertexPositionNormalTangentTexture&) = default;
		VertexPositionNormalTangentTexture& operator=(const VertexPositionNormalTangentTexture&) = default;

		VertexPositionNormalTangentTexture(VertexPositionNormalTangentTexture&&) = default;
		VertexPositionNormalTangentTexture& operator=(VertexPositionNormalTangentTexture&&) = default;

		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT2 textureCoordinate;

		VertexPositionNormalTangentTexture(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT3 const& tangent, XMFLOAT2 const& textureCoordinate)
			: position(position),
			normal(normal),
			tangent(tangent),
			textureCoordinate(textureCoordinate)
		{
		}

		VertexPositionNormalTangentTexture(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR tangent, CXMVECTOR textureCoordinate)
		{
			XMStoreFloat3(&this->position, position);
			XMStoreFloat3(&this->normal, normal);
			XMStoreFloat3(&this->tangent, tangent);
			XMStoreFloat2(&this->textureCoordinate, textureCoordinate);
		}

		static const int InputElementCount = 4;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}