#include "pch.h"
#include "FPSRenderer.h"
#include <string>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace Bruce
{
	//!
	void FPSRenderer::CreateDeviceDependentResources(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		m_font = std::make_unique<DirectX::SpriteFont>(device, L"asset/CourierNew.spritefont");
		m_spriteBatch = std::make_unique<SpriteBatch>(context);
	}

	//!
	void FPSRenderer::Update(DX::StepTimer const& timer)
	{
		uint32_t fps = timer.GetFramesPerSecond();
		
		m_text = L"FPS : " + std::to_wstring(fps);
	}

	//!
	void FPSRenderer::Render(ID3D11DeviceContext* context)
	{
		m_spriteBatch->Begin();

		Vector2 origin = m_font->MeasureString(m_text.c_str()) / 2.f;
		m_fontPos = origin;

		m_font->DrawString(m_spriteBatch.get(), m_text.c_str(),	m_fontPos, Colors::White, 0.f, origin);

		m_spriteBatch->End();
	}

	//!
	void FPSRenderer::OnDeviceLost()
	{
		m_font.reset();
		m_spriteBatch.reset();
	}

}