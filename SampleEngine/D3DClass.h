// filename: D3DClass.h

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

// link required libraries
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

// dx11 related headers
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>		// try commenting this out

// for DirectXMath functions
using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass & other);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float r, float g, float b, float a);
	void EndScene();

	ID3D11Device * GetDevice();
	ID3D11DeviceContext * GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX & projectionMatrix);
	void GetWorldMatrix(XMMATRIX & worldMatrix);
	void GetOrthoMatrix(XMMATRIX & orthoMatrix);
	
	void GetVideoCardInfo(char * cardName, int & memory);

private:
	bool mVsyncEnabled;
	int mVideoCardMemory;
	char mVideoCardDescription[128];
	IDXGISwapChain * mSwapChain;
	ID3D11Device * mDevice;
	ID3D11DeviceContext * mDeviceContext;
	ID3D11RenderTargetView * mRenderTargetView;
	ID3D11Texture2D * mDepthStencilBuffer;
	ID3D11DepthStencilState * mDepthStencilState;
	ID3D11DepthStencilView * mDepthStencilView;
	ID3D11RasterizerState * mRasterizerState;
	XMMATRIX mProjectionMatrix;
	XMMATRIX mWorldMatrix;
	XMMATRIX mOrthoMatrix;
};

#endif // !_D3DCLASS_H_



