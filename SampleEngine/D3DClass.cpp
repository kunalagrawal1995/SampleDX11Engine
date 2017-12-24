// filename: D3DClass.cpp

#include "D3DClass.h"

D3DClass::D3DClass()
{
	mSwapChain = 0;
	mDevice = 0;
	mDeviceContext = 0;
	mRenderTargetView = 0;
	mDepthStencilBuffer = 0;
	mDepthStencilState = 0;
	mDepthStencilView = 0;
	mRasterizerState = 0;
}

D3DClass::D3DClass(const D3DClass & other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory * factory;
	IDXGIAdapter * adapter;
	IDXGIOutput * adapterOutput;
	unsigned int numModes;
	DXGI_MODE_DESC * displayModeList;
	unsigned int numerator, denominator;	//used for screen refresh rate
	DXGI_ADAPTER_DESC adapterDesc;
	unsigned int stringLength;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	UINT flags;
	ID3D11Texture2D * backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	// store vsync value
	mVsyncEnabled = vsync;

	// create factory interface, to find the refresh rate of monitor
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// use factory to create adapter for primary graphics card
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// enumerate the primary adapter output (monitor)
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// get number of modes that fit DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// create list to hold all possible display modes for monitor/graphics card combination
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// fill display mode list structure
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// go through all display modes and find the one matching screen dimentions and store its refresh rate
	for (int i = 0; i < (int)numModes; ++i)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth && displayModeList[i].Height == (unsigned int)screenHeight)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// get adapter description to get its name and memory
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// store video card memory
	mVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	
	// store video card name
	error = wcstombs_s(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// release the structures used to get the refresh rate
	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	factory->Release();
	factory = 0;

	// fill out swap chain description
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 1 buffer
	swapChainDesc.BufferCount = 1;

	// fill buffer desc
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// if vsync is true, sync refresh rate with monitor; else go as fast as possible
	if (mVsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// usage
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// window handle
	swapChainDesc.OutputWindow = hwnd;

	// turn multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// set full screen or windowed mode
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// no flags
	swapChainDesc.Flags = 0;

	// swap effect
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// feature level
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// flags
	flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// create swap chain, device, and device context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &mSwapChain, &mDevice, NULL, &mDeviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// get back buffer
	result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// create render target view of back buffer
	result = mDevice->CreateRenderTargetView(backBufferPtr, NULL, &mRenderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// release pointer to the back buffer
	backBufferPtr->Release();
	backBufferPtr = 0;

	// create depth stencil buffer description
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.MiscFlags = 0;

	// create depth stencil texture
	result = mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// setup depth stencil description
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// stencil operations if pixel is front facing
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// stencil operations if pixel is back facing
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	// create depth stencil state
	result = mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// set depth stencil state
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);
	
	// create depth stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &depthStencilViewDesc, &mDepthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// bind render target view and depth stencil view to OM stage of pipeline
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// setup rasterizer description
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// create rasterizer state
	result = mDevice->CreateRasterizerState(&rasterizerDesc, &mRasterizerState);
	if (FAILED(result))
	{
		return false;
	}

	// set rasterizer state
	mDeviceContext->RSSetState(mRasterizerState);

	// setup viewport for rendering
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// set viewport
	mDeviceContext->RSSetViewports(1, &viewport);

	// setup projection matrix
	fieldOfView = (float)XM_PIDIV4;
	screenAspect = (float)screenWidth / (float)screenHeight;

	mProjectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// initialize world matrix to identity
	mWorldMatrix = XMMatrixIdentity();

	// create orthographic projection matrix
	mOrthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// force windowed mode to avoid errors while releasing swap chain in full screen mode
	if (mSwapChain)
	{
		mSwapChain->SetFullscreenState(false, NULL);
	}

	if (mRasterizerState)
	{
		mRasterizerState->Release();
		mRasterizerState = 0;
	}

	if (mDepthStencilView)
	{
		mDepthStencilView->Release();
		mDepthStencilView = 0;
	}

	if (mDepthStencilState)
	{
		mDepthStencilState->Release();
		mDepthStencilState = 0;
	}

	if (mDepthStencilBuffer)
	{
		mDepthStencilBuffer->Release();
		mDepthStencilBuffer = 0;
	}

	if (mRenderTargetView)
	{
		mRenderTargetView->Release();
		mRenderTargetView = 0;
	}

	if (mDeviceContext)
	{
		mDeviceContext->Release();
		mDeviceContext = 0;
	}

	if (mDevice)
	{
		mDevice->Release();
		mDevice = 0;
	}

	if (mSwapChain)
	{
		mSwapChain->Release();
		mSwapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float r, float g, float b, float a)
{
	float color[4] = { r, g, b, a };

	// clear back buffer
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, color);

	// clear depth stencil view
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	if (mVsyncEnabled)
	{
		// lock to screen refresh rate
		mSwapChain->Present(1, 0);
	}
	else
	{
		// render as fast as possible
		mSwapChain->Present(0, 0);
	}

	return;
}

ID3D11Device * D3DClass::GetDevice()
{
	return mDevice;
}

ID3D11DeviceContext * D3DClass::GetDeviceContext()
{
	return mDeviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX & projectionMatrix)
{
	projectionMatrix = mProjectionMatrix;
}

void D3DClass::GetWorldMatrix(XMMATRIX & worldMatrix)
{
	worldMatrix = mWorldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX & orthoMatrix)
{
	orthoMatrix = mOrthoMatrix;
}

void D3DClass::GetVideoCardInfo(char * cardName, int & memory)
{
	strcpy_s(cardName, 128, mVideoCardDescription);
	memory = mVideoCardMemory;
	
	return;
}