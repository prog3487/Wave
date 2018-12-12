#pragma once

#include "ConstantBuffer.h"
#include "CommonStates.h"
#include "SimpleMath.h"
#include "DirectXHelpers.h"

namespace Bruce
{
	//!
	struct DeviceResource
	{
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		std::unique_ptr<DirectX::CommonStates> mCommonStates;
	};

	//!
	template<typename ConstantBufferType>
	class EffectBase
	{
	public:
		EffectBase(ID3D11Device* device)
		{
			if (!mDeviceResource.device)
			{
				mDeviceResource.device = device;
				mDeviceResource.mCommonStates = std::make_unique<DirectX::CommonStates>(device);
			}

			mConstantBuffer.Create(mDeviceResource.device.Get());
		}

		void SetMatrices(DirectX::FXMMATRIX world, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
		{
			mWorld = world;
			mView = view;
			mProj = projection;
		}

	protected:
		DirectX::SimpleMath::Matrix mWorld;
		DirectX::SimpleMath::Matrix mView;
		DirectX::SimpleMath::Matrix mProj;

		ConstantBufferType mCPUBuffer;
		ConstantBuffer<ConstantBufferType> mConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	mVS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	mPS;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	mInputLayout;

		static DeviceResource mDeviceResource;
	};

	//!
	template<typename ConstantBufferType>
	DeviceResource EffectBase<ConstantBufferType>::mDeviceResource;
}
