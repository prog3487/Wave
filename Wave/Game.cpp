//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "ReadData.h"
#include "DDSTextureLoader.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(1600),
    m_outputHeight(900),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

	m_fake_eye_camera = std::make_unique<Bruce::Camera>();
	m_CurrTessCamera = m_fake_eye_camera.get();

	m_Camera = std::make_unique<Bruce::Camera>();
	m_Camera->CreateView(Vector3(0, 10, 10), Vector3::Zero, Vector3::UnitY);

	{	// Wave material
		m_wave_mat_buffer.DiffuseAlbedo = Colors::DodgerBlue;
		m_wave_mat_buffer.FresnelR0 = { 0.02f, 0.02f, 0.02f };
		m_wave_mat_buffer.Roughness = 0.02f;
	}

	{	// Light settings
		Vector3 LightPos(10.0f, 10.0f, -10.0f);
		Vector3 LightDir = Vector3::Zero - LightPos;
		LightDir.Normalize();

		m_light_buffer.DirLight.ColorStrength = { 1.0f, 1.0f, 1.0f };
		m_light_buffer.DirLight.Direction = LightDir;
		//m_light_buffer.DirLight.Direction = -Vector3::UnitY;
	}
	
	CreateGrid(30.0f, 30.0f, 80, 80, m_grid);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);

	m_wave_world = Matrix::Identity;
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

static const float CAMERA_MOVE_SPEED = 10.0f;
static const float CAMERA_ROTATE_SPEED = 0.25f;

void Game::UpdateInput(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());

	{	// keyboard
		auto kb = m_keyboard->GetState();

		{	// camera control
			float moveDelta = CAMERA_MOVE_SPEED * elapsedTime;
			if (kb.LeftShift)
			{
				moveDelta *= 0.05f;
			}
			if (kb.W)
			{
				m_Camera->Walk(moveDelta);
			}
			if (kb.S)
			{
				m_Camera->Walk(-moveDelta);
			}
			if (kb.A)
			{
				m_Camera->Strafe(-moveDelta);
			}
			if (kb.D)
			{
				m_Camera->Strafe(moveDelta);
			}
			if (kb.Q)
			{
				m_Camera->Fly(moveDelta);
			}
			if (kb.E)
			{
				m_Camera->Fly(-moveDelta);
			}
		}

		{	// change tessellation camera
			if (kb.D1)
			{
				m_CurrTessCamera = m_fake_eye_camera.get();
			}
			else if (kb.D2)
			{
				m_CurrTessCamera = m_Camera.get();
			}
		}

		{	// change RasterizerState
			if (kb.D3)
				m_CurrRS = m_common_states->Wireframe();
			if (kb.D4)
				m_CurrRS = m_common_states->CullCounterClockwise();
		}
	}

	{	// mouse
		auto mouse = m_mouse->GetState();
		if (mouse.positionMode == Mouse::MODE_RELATIVE)
		{
			Vector3 delta = Vector3(float(mouse.x), float(mouse.y), 0.f) * CAMERA_ROTATE_SPEED;
			Vector3 deltaRadian(XMConvertToRadians(delta.x), XMConvertToRadians(delta.y), 0);

			m_Camera->Pitch(-deltaRadian.y);
			m_Camera->RotateY(-deltaRadian.x);
		}

		m_mouse->SetMode(mouse.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	}
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	m_fps_renderer.Update(timer);

	UpdateInput(timer);

	m_Camera->UpdateViewMatrix();

	float elapsedTime = float(timer.GetElapsedSeconds());

	{
		m_wave1_offset.x += elapsedTime * 0.01f;
		m_wave1_offset.y += elapsedTime * 0.03f;
		m_wave2_offset.x += elapsedTime * 0.01f;
		m_wave2_offset.y += elapsedTime * 0.03f;

		m_normal1_offset.x += elapsedTime * 0.05f;
		m_normal1_offset.y += elapsedTime * 0.2f;
		m_normal2_offset.x += elapsedTime * 0.02f;
		m_normal2_offset.y += elapsedTime * 0.05f;

		static UINT PixelsPerEdge = 50;
		m_wave_buffer.World = m_wave_world;
		m_wave_buffer.ViewProj = m_Camera->GetView() * m_Camera->GetProj();
		m_wave_buffer.TessViewProj = m_CurrTessCamera->GetView() * m_CurrTessCamera->GetProj();
		m_wave_buffer.Proj11 = m_CurrTessCamera->GetProj()._11;
		m_wave_buffer.EdgesPerScreenHeight = (float)m_outputHeight / PixelsPerEdge;
		m_wave_buffer.HeightScale = 0.3f;
		
		m_wave_buffer.DisplaceTexTransform0 = 
			Matrix::CreateScale(2.0f) *
			Matrix::CreateTranslation(m_wave1_offset.x, m_wave1_offset.y, 0.0f);
		m_wave_buffer.DisplaceTexTransform1 = 
			Matrix::CreateScale(1.0f) *
			Matrix::CreateTranslation(m_wave2_offset.x, m_wave2_offset.y, 0.0f);

		m_wave_buffer.NormalTexTransform0 =
			Matrix::CreateScale(22.0f) *
			Matrix::CreateTranslation(m_normal1_offset.x, m_normal1_offset.y, 0.0f);
		m_wave_buffer.NormalTexTransform1=
			Matrix::CreateScale(1.0f) *
			Matrix::CreateTranslation(m_normal2_offset.x, m_normal2_offset.y, 0.0f);
	}

	{
		m_light_buffer.EyePosW = m_Camera->GetPos();
	}
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

	UINT stride = m_grid.Stride();
	UINT offset = 0;
	
	m_d3dContext->OMSetBlendState(m_common_states->Opaque(), nullptr, 0xFFFFFFFF);
	m_d3dContext->OMSetDepthStencilState(m_common_states->DepthDefault(), 0);

	m_d3dContext->IASetInputLayout(m_wave_layout.Get());
	m_d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	m_d3dContext->IASetVertexBuffers(0, 1, m_grid.VB.GetAddressOf(), &stride, &offset);
	m_d3dContext->IASetIndexBuffer(m_grid.IB.Get(), m_grid.GetIndexDXGIFormat(), 0);
	
	m_d3dContext->RSSetState(m_CurrRS);

	m_d3dContext->VSSetShader(m_wave_vs.Get(), nullptr, 0);
	m_d3dContext->HSSetShader(m_wave_hs.Get(), nullptr, 0);
	m_d3dContext->DSSetShader(m_wave_ds.Get(), nullptr, 0);
	m_d3dContext->PSSetShader(m_wave_ps.Get(), nullptr, 0);

	m_wave_cbuffer.SetData(m_d3dContext.Get(), m_wave_buffer);
	m_light_cbuffer.SetData(m_d3dContext.Get(), m_light_buffer);
	m_wave_mat_cbuffer.SetData(m_d3dContext.Get(), m_wave_mat_buffer);

	m_d3dContext->VSSetConstantBuffers(0, 1, m_wave_cbuffer.GetAddressOf());
	m_d3dContext->DSSetConstantBuffers(0, 1, m_wave_cbuffer.GetAddressOf());
	m_d3dContext->HSSetConstantBuffers(0, 1, m_wave_cbuffer.GetAddressOf());
	
	ID3D11Buffer* cbuffers[] = { m_light_cbuffer.GetBuffer(), m_wave_mat_cbuffer.GetBuffer() };
	m_d3dContext->PSSetConstantBuffers(0, _countof(cbuffers), cbuffers);

	ID3D11ShaderResourceView* textures[] = { m_wave_tex_SRV1.Get(), m_wave_tex_SRV2.Get() };
	ID3D11SamplerState* samplers[] = { m_common_states->LinearWrap() };
	m_d3dContext->DSSetShaderResources(0, _countof(textures), textures);
	m_d3dContext->DSSetSamplers(0, _countof(samplers), samplers);

	ID3D11ShaderResourceView* texturesPS[] = { m_wave_tex_SRV1.Get(), m_wave_tex_SRV2.Get(), m_CubeMap.Get() };
	ID3D11SamplerState* samplersPS[] = { m_common_states->LinearWrap(), m_common_states->LinearWrap() };
	m_d3dContext->PSSetShaderResources(0, _countof(texturesPS), texturesPS);
	m_d3dContext->PSSetSamplers(0, _countof(samplersPS), samplersPS);

	//-
	m_d3dContext->DrawIndexed(m_grid.indices.size(), 0, 0);
	//m_d3dContext->DrawIndexed(12, 0, 0);
	//-

	m_d3dContext->VSSetShader(nullptr, nullptr, 0);
	m_d3dContext->HSSetShader(nullptr, nullptr, 0);
	m_d3dContext->DSSetShader(nullptr, nullptr, 0);
	m_d3dContext->PSSetShader(nullptr, nullptr, 0);

	// 
	m_fake_eye_obj->Draw(m_fake_eye_world, m_Camera->GetView(), m_Camera->GetProj());

	// draw sky last
	auto skyWorld = Matrix::CreateTranslation(m_Camera->GetPos());
	m_sky.Render(m_d3dContext.Get(), skyWorld, m_Camera->GetView(), m_Camera->GetProj());

	//
	m_fps_renderer.Render(m_d3dContext.Get());

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(0, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1600;
    height = 900;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
        ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    // Initialize device dependent objects here (independent of window size).
	
	m_common_states = std::make_unique<DirectX::CommonStates>(m_d3dDevice.Get());
	m_CurrRS = m_common_states->CullCounterClockwise();

	assert(!m_grid.vertices.empty() && "grid must be created first!");

	{	// grid vertex buffer
		m_grid.CreateVertexBuffer(device.Get());
	}

	{	// grid index buffer
		m_grid.CreateIndexBuffer(device.Get());
	}

	{	// create shaders
		auto blob_vs = DX::ReadData(L"wave_vs.cso");
		DX::ThrowIfFailed(
			device->CreateVertexShader(blob_vs.data(), blob_vs.size(), nullptr, m_wave_vs.ReleaseAndGetAddressOf()));

		auto blob_hs = DX::ReadData(L"wave_hs.cso");
		DX::ThrowIfFailed(
			device->CreateHullShader(blob_hs.data(), blob_hs.size(), nullptr, m_wave_hs.ReleaseAndGetAddressOf()));

		auto blob_ds = DX::ReadData(L"wave_ds.cso");
		DX::ThrowIfFailed(
			device->CreateDomainShader(blob_ds.data(), blob_ds.size(), nullptr, m_wave_ds.ReleaseAndGetAddressOf()));

		auto blob_ps = DX::ReadData(L"wave_ps.cso");
		DX::ThrowIfFailed(
			device->CreatePixelShader(blob_ps.data(), blob_ps.size(), nullptr, m_wave_ps.ReleaseAndGetAddressOf()));

		// layout
		device->CreateInputLayout(
			VertexPositionNormalTangentTexture::InputElements,
			VertexPositionNormalTangentTexture::InputElementCount,
			blob_vs.data(), blob_vs.size(), 
			m_wave_layout.ReleaseAndGetAddressOf());
	}

	m_wave_cbuffer.Create(device.Get());
	m_light_cbuffer.Create(device.Get());
	m_wave_mat_cbuffer.Create(device.Get());

	m_fake_eye_obj = DirectX::GeometricPrimitive::CreateTeapot(m_d3dContext.Get(), 0.5f);

	{
		DX::ThrowIfFailed(
			DirectX::CreateDDSTextureFromFile(
				device.Get(), /*L"asset/water_normal.dds"*/ L"asset/waves0.dds", nullptr, m_wave_tex_SRV1.ReleaseAndGetAddressOf()));

		DX::ThrowIfFailed(
			DirectX::CreateDDSTextureFromFile(
				device.Get(), /*L"asset/water_diffuse.dds"*/L"asset/waves1.dds", nullptr, m_wave_tex_SRV2.ReleaseAndGetAddressOf()));

		DX::ThrowIfFailed(
			DirectX::CreateDDSTextureFromFile(
				device.Get(), L"asset/sunsetcube1024.dds", nullptr, m_CubeMap.ReleaseAndGetAddressOf()));
	}

	{
		m_wave_mat_cbuffer.SetData(m_d3dContext.Get(), m_wave_mat_buffer);
	}

	m_sky.InitDeviceDependentResources(device.Get());
	m_fps_renderer.CreateDeviceDependentResources(device.Get(), context.Get());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
            ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // Initialize windows-size dependent objects here.
	m_Camera->CreateProj(XM_PIDIV4, float(m_outputWidth) / float(m_outputHeight), 0.01f, 1000.0f);

	
	{	// fake eye settings
		Vector3 eye(0, 1, 10);
		m_fake_eye_world = Matrix::CreateWorld(eye, -eye, Vector3::UnitY);
		m_fake_eye_camera->CreateView(eye, Vector3::Zero, Vector3::UnitY);
		m_fake_eye_camera->CreateProj(XM_PIDIV4, float(m_outputWidth) / float(m_outputHeight), 0.01f, 1000.0f);
		m_fake_eye_camera->UpdateViewMatrix();
	}
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
	m_fps_renderer.OnDeviceLost();

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}

void Game::CreateGrid(float width, float height, UINT row, UINT col, WaveGeometry_t& result_geometry)
{
	UINT vertexCount = row * col;
	UINT faceCount = (row - 1)*(col - 1) * 2;

	//
	// Create the vertices.
	//

	float halfWidth = 0.5f*width;
	float halfHeight = 0.5f*height;

	float dx = width / (col - 1);
	float dz = height / (row - 1);

	float du = 1.0f / (col - 1);
	float dv = 1.0f / (row - 1);

	result_geometry.vertices.resize(vertexCount);
	for (UINT i = 0; i < row; ++i)
	{
		float z = -halfHeight + i * dz;
		for (UINT j = 0; j < col; ++j)
		{
			float x = -halfWidth + j * dx;

			result_geometry.vertices[i*col + j].position	= XMFLOAT3(x, 0.0f, z);
			result_geometry.vertices[i*col + j].normal		= XMFLOAT3(0.0f, 1.0f, 0.0f);
			result_geometry.vertices[i*col + j].tangent		= XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Stretch texture over grid.
			result_geometry.vertices[i*col + j].textureCoordinate.x = j * du;
			result_geometry.vertices[i*col + j].textureCoordinate.y = i * dv;
		}
	}

	//
	// Create the indices.
	//

	result_geometry.indices.resize(faceCount * 3); // 3 indices per face

	// Iterate over each quad and compute indices.
	UINT k = 0;
	for (UINT i = 0; i < row - 1; ++i)
	{
		for (UINT j = 0; j < col - 1; ++j)
		{
			result_geometry.indices[k] = i * col + j;				//0
			result_geometry.indices[k + 1] = i * col + j + 1;		//1
			result_geometry.indices[k + 2] = (i + 1)*col + j;		//2

			result_geometry.indices[k + 3] = (i + 1)*col + j + 1;	//3
			result_geometry.indices[k + 4] = (i + 1)*col + j;		//2
			result_geometry.indices[k + 5] = i * col + j + 1;		//1

			k += 6; // next quad
		}
	}
}