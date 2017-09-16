#include "stdafx.h"
#include "Snuggling.h"
#include "Duplicator.h"

#define ReturnIfFailed(hr) do { HRESULT hr_= (hr); if(FAILED(hr_)) return hr_; } while(false)

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

static inline void ThrowIfFailed(HRESULT hr) {
	if(FAILED(hr)) {
		_com_issue_error(hr);
	}
}

Duplicator::Duplicator() {
	HRESULT hr;
#ifdef _DEBUG
	UINT const flags= D3D11_CREATE_DEVICE_DEBUG;
#else
	UINT const flags= 0;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	hr= D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		nullptr, 0, D3D11_SDK_VERSION, &pDevice, &featureLevel, &pContext);
	ThrowIfFailed(hr);
	hr= pDevice.As(&pDevice1);
	ThrowIfFailed(hr);
	hr= pDevice1->GetParent(IID_PPV_ARGS(&pAdapter1));
	ThrowIfFailed(hr);
	hr= pAdapter1->EnumOutputs(0, &pOutput);
	ThrowIfFailed(hr);
	hr= pOutput.As(&pOutput1);
	ThrowIfFailed(hr);
	hr= pOutput1->DuplicateOutput(pDevice.Get(), &pDuplication);
	ThrowIfFailed(hr);
}

Duplicator::~Duplicator() {}

table_id_t Duplicator::Doit(int y, int width, int height) {
	HRESULT hr;
	if(pDuplication == nullptr) {
		hr= pOutput1->DuplicateOutput(pDevice.Get(), &pDuplication);
		ThrowIfFailed(hr);
	}
	DXGI_OUTDUPL_FRAME_INFO info;
	ComPtr<IDXGIResource> pResource;
	hr= pDuplication->AcquireNextFrame(110, &info, &pResource);
	while(hr == DXGI_ERROR_WAIT_TIMEOUT) {
		OutputDebugString(_T("wait time-out\n"));
		hr= pDuplication->AcquireNextFrame(110, &info, &pResource);
	}
	DXGI_OUTDUPL_DESC ddesc;
	pDuplication->GetDesc(&ddesc);
#ifdef _DEBUG
	TCHAR sz[999];
	wsprintf(sz, _T("w %d, h %d\n"), ddesc.ModeDesc.Width, ddesc.ModeDesc.Height);
	OutputDebugString(sz);
#endif
	while(hr == DXGI_ERROR_ACCESS_LOST) {
		OutputDebugString(_T("access lost\n"));
		Sleep(1100);
		hr= pOutput1->DuplicateOutput(pDevice.Get(), &pDuplication);
		ThrowIfFailed(hr);
		hr= pDuplication->AcquireNextFrame(0, &info, &pResource);
	}
	ThrowIfFailed(hr);
	LPCTSTR s= info.LastPresentTime.QuadPart ? _T("updated\n") : _T("not updated\n");
	OutputDebugString(s);
	// https://stackoverflow.com/questions/22383209/id3d11texture2d-to-id2d1bitmap-is-it-possible/22397989#22397989
	ComPtr<ID3D11Texture2D> pSourceTexture;
	hr= pResource.As(&pSourceTexture);
	ThrowIfFailed(hr);
	D3D11_TEXTURE2D_DESC textureDesc;
	pSourceTexture->GetDesc(&textureDesc);
	textureDesc.Usage= D3D11_USAGE_STAGING;
	textureDesc.BindFlags= 0;
	textureDesc.CPUAccessFlags= D3D11_CPU_ACCESS_READ;
	textureDesc.MiscFlags= 0;
	textureDesc.Width= width;
	textureDesc.Height= height;
	ComPtr<ID3D11Texture2D> pTargetTexture;
	hr= pDevice->CreateTexture2D(&textureDesc, nullptr, &pTargetTexture);
	ReturnIfFailed(hr);
	D3D11_BOX box{};
	box.front= 0;
	box.back= 1;
	box.top= y;
	box.bottom= y + textureDesc.Height;
	box.left= (ddesc.ModeDesc.Width - textureDesc.Width) / 2;
	box.right= box.left + textureDesc.Width;
	pContext->CopySubresourceRegion(pTargetTexture.Get(), 0, 0, 0, 0, pSourceTexture.Get(), 0, &box);
	ComPtr<IDXGISurface> pSurface;
	hr= pTargetTexture.As(&pSurface);
	ThrowIfFailed(hr);
	DXGI_SURFACE_DESC desc;
	hr= pSurface->GetDesc(&desc);
	ThrowIfFailed(hr);
	DXGI_MAPPED_RECT rect{};
	hr= pSurface->Map(&rect, DXGI_MAP_READ);
	ThrowIfFailed(hr);
	auto tableId= HashData(rect.pBits, rect.Pitch * desc.Height);
	hr= pSurface->Unmap();
	hr= pDuplication->ReleaseFrame();
	return tableId;
}
