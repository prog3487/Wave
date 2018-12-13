#pragma once

#include "SpriteFont.h"
#include "StepTimer.h"

namespace Bruce
{

	class FPSRenderer
	{
	public:
		void CreateDeviceDependentResources(ID3D11Device* device, ID3D11DeviceContext* context);
		void Update(DX::StepTimer const& timer);
		void Render(ID3D11DeviceContext* context);
		void OnDeviceLost();

	protected:
		std::unique_ptr<DirectX::SpriteFont> m_font;
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch; 
		DirectX::SimpleMath::Vector2 m_fontPos;
		std::wstring m_text;
		
	};


}