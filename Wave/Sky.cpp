#include "pch.h"
#include "Sky.h"
#include "GeometricPrimitive.h"

using namespace DirectX;

namespace Bruce
{
	//!
	void Sky::InitDeviceDependentResources(ID3D11Device* device)
	{
		m_SkyEffect = std::make_unique<Bruce::EffectSky>(device);
		m_SkyEffect->LoadCubeMap(L"asset/grasscube1024.dds");
		
		std::vector<GeometricPrimitive::VertexType> vertices;
		GeometricPrimitive::CreateGeoSphere(vertices, m_sphere.indices);

		m_sphere.vertices.resize(vertices.size());
		for (size_t i=0; i<vertices.size(); ++i)
		{
			m_sphere.vertices[i].position = vertices[i].position;
		}

		m_sphere.CreateVertexBuffer(device);
		m_sphere.CreateIndexBuffer(device);
	}

	//!
	void Sky::Render(ID3D11DeviceContext* context,
		const DirectX::SimpleMath::Matrix& world, 
		const DirectX::SimpleMath::Matrix& view, 
		const DirectX::SimpleMath::Matrix& proj)
	{
		m_SkyEffect->SetMatrices(world, view, proj);
		m_SkyEffect->Apply(context);
		
		UINT stride = m_sphere.Stride(), offsets = 0;
		context->IASetVertexBuffers(0, 1, m_sphere.VB.GetAddressOf(), &stride, &offsets);
		context->IASetIndexBuffer(m_sphere.IB.Get(), m_sphere.GetIndexDXGIFormat(), 0);

		context->DrawIndexed(m_sphere.indices.size(), 0, 0);
	}

}
