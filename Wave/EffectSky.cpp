#include "pch.h"
#include "IEffect.h"
#include "DDSTextureLoader.h"
#include "ReadData.h"
#include "VertexTypes.h"
#include "EffectCommon.h"
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace Bruce
{
	//!
	struct SkyConstantBuffer
	{
		Matrix gWVP;
	};

	//!
	class EffectSky::Impl : public EffectBase<SkyConstantBuffer>
	{
	public:
		Impl(ID3D11Device* device);
		void Apply(ID3D11DeviceContext* context);
		void LoadCubeMap(std::wstring path);

	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	mCubeMap;
	};

	EffectSky::Impl::Impl(ID3D11Device* device)
		:EffectBase(device)
	{
		{
			auto blob = DX::ReadData(L"sky_vs.cso");
			DX::ThrowIfFailed(
				mDeviceResource.device->CreateVertexShader(blob.data(), blob.size(), nullptr, mVS.ReleaseAndGetAddressOf()));

			DX::ThrowIfFailed(
				mDeviceResource.device->CreateInputLayout(
					VertexPosition::InputElements,
					VertexPosition::InputElementCount,
					blob.data(), blob.size(),
					mInputLayout.ReleaseAndGetAddressOf()));
		}

		{
			auto blob = DX::ReadData(L"sky_ps.cso");
			DX::ThrowIfFailed(
				mDeviceResource.device->CreatePixelShader(blob.data(), blob.size(), nullptr, mPS.ReleaseAndGetAddressOf()));
		}
	}

	//!
	void EffectSky::Impl::Apply(ID3D11DeviceContext* context)
	{
		context->RSSetState(mDeviceResource.mCommonStates->CullClockwise());
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(mInputLayout.Get());
		context->VSSetShader(mVS.Get(), nullptr, 0);
		context->PSSetShader(mPS.Get(), nullptr, 0);
		context->PSSetShaderResources(0, 1, mCubeMap.GetAddressOf());
		mCPUBuffer.gWVP = mWorld * mView * mProj;
		mConstantBuffer.SetData(context, mCPUBuffer);
		context->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
	}

	//!
	void EffectSky::Impl::LoadCubeMap(std::wstring path)
	{
		DX::ThrowIfFailed(
			CreateDDSTextureFromFileEx(mDeviceResource.device.Get(), path.c_str(), 0, 
				D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE,
				false, nullptr, mCubeMap.ReleaseAndGetAddressOf()));
	}

	//!
	void EffectSky::SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
	{
		pImpl->SetMatrices(world, view, projection);
	}

	//!
	EffectSky::EffectSky(ID3D11Device* device)
		:pImpl(std::make_unique<EffectSky::Impl>(device))
	{

	}

	//!
	void EffectSky::Apply(ID3D11DeviceContext* context)
	{
		pImpl->Apply(context);
	}

	//!
	void EffectSky::LoadCubeMap(std::wstring path)
	{
		pImpl->LoadCubeMap(path);
	}

	//!
	EffectSky::~EffectSky()
	{
		
	}

}