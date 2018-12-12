#pragma once


namespace Bruce
{
	//!
	class IEffect
	{
	public:
		virtual void Apply(ID3D11DeviceContext* context) = 0;
		virtual void SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) = 0;
	};

	//!
	class EffectSky : public IEffect
	{
	public:
		EffectSky(ID3D11Device* device);
		~EffectSky();

		void Apply(ID3D11DeviceContext* context) override;
		void SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection) override;
		void LoadCubeMap(std::wstring path);


	private:
		class Impl;
		std::unique_ptr<Impl> pImpl;
	};



}