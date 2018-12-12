//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include "SimpleMath.h"
#include "Geometry.h"
#include "CommonStates.h"
#include "Camera.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "ConstantBuffer.h"
#include "GeometricPrimitive.h"
#include "BVertexTypes.h"
#include "Sky.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

	void UpdateInput(DX::StepTimer const& timer);

	// Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

	using WaveGeometry_t = Bruce::Geometry<DirectX::VertexPositionNormalTangentTexture>;
	
	void CreateGrid(float width, float height, UINT m, UINT n, WaveGeometry_t& result_geometry);

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                   m_timer;

	//
	std::unique_ptr<Bruce::Camera>					m_Camera;
	std::unique_ptr<DirectX::Keyboard>				m_keyboard;
	std::unique_ptr<DirectX::Mouse>					m_mouse;

	// 
	std::unique_ptr<DirectX::CommonStates>			m_common_states;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_wave_layout;

	// Geometry for wave
	WaveGeometry_t									m_grid;

	// wave shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_wave_vs;
	Microsoft::WRL::ComPtr<ID3D11HullShader>		m_wave_hs;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>		m_wave_ds;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_wave_ps;

	// constant buffer
	struct WaveBuffer
	{
		DirectX::SimpleMath::Matrix World;
		DirectX::SimpleMath::Matrix ViewProj;
		DirectX::SimpleMath::Matrix TessViewProj;
		DirectX::SimpleMath::Matrix DisplaceTexTransform0;
		DirectX::SimpleMath::Matrix DisplaceTexTransform1;
		DirectX::SimpleMath::Matrix NormalTexTransform0;
		DirectX::SimpleMath::Matrix NormalTexTransform1;
		float Proj11;
		float EdgesPerScreenHeight;
		float HeightScale;
		float pad0;
	}; 

	struct DirectionalLight
	{
		DirectX::SimpleMath::Vector3 ColorStrength;
		float pad0;
		DirectX::SimpleMath::Vector3 Direction;
		float pad1;
	};

	struct Material
	{
		DirectX::SimpleMath::Vector4 DiffuseAlbedo;
		DirectX::SimpleMath::Vector3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
	};

	struct LightBuffer
	{
		DirectionalLight DirLight;
		DirectX::SimpleMath::Vector3 EyePosW;
		float pad1;
	};

	LightBuffer										m_light_buffer;
	Bruce::ConstantBuffer<LightBuffer>				m_light_cbuffer;

	Material										m_wave_mat_buffer;
	Bruce::ConstantBuffer<Material>					m_wave_mat_cbuffer;

	WaveBuffer										m_wave_buffer;
	Bruce::ConstantBuffer<WaveBuffer>				m_wave_cbuffer;
	DirectX::SimpleMath::Matrix						m_wave_world;
	DirectX::SimpleMath::Vector2					m_wave1_offset = DirectX::SimpleMath::Vector2::Zero;
	DirectX::SimpleMath::Vector2					m_wave2_offset = DirectX::SimpleMath::Vector2::Zero;
	DirectX::SimpleMath::Vector2					m_normal1_offset = DirectX::SimpleMath::Vector2::Zero;
	DirectX::SimpleMath::Vector2					m_normal2_offset = DirectX::SimpleMath::Vector2::Zero;


	//
	std::unique_ptr<DirectX::GeometricPrimitive>	m_fake_eye_obj;
	DirectX::SimpleMath::Matrix						m_fake_eye_world;
	std::unique_ptr<Bruce::Camera>					m_fake_eye_camera;

	// Game related
	Bruce::Camera* m_CurrTessCamera = nullptr;
	ID3D11RasterizerState* m_CurrRS = nullptr;

	//
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_wave_tex_SRV1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_wave_tex_SRV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_CubeMap;

	//
	Bruce::Sky m_sky;
};