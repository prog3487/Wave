#pragma once

#include "SimpleMath.h"
#include "ConstantBuffer.h"
#include "StepTimer.h"
#include "Geometry.h"
#include "IEffect.h"

namespace Bruce
{
	class Sky
	{
	public:

		void InitDeviceDependentResources(ID3D11Device* device);
		void Render(ID3D11DeviceContext* context,
			const DirectX::SimpleMath::Matrix& world,
			const DirectX::SimpleMath::Matrix& view,
			const DirectX::SimpleMath::Matrix& proj);


	private:

		std::unique_ptr<Bruce::EffectSky> m_SkyEffect;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_VB;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_IB;

		using vtype = DirectX::VertexPosition;
		Bruce::Geometry<vtype, uint16_t> m_sphere;
	};
}