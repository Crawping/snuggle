#include "stdafx.h"
#include "Snuggling.h"

static HDC sourceContext, targetContext;
static int bitCount;
static unsigned size;
static void* data;
static HWND window;
static int x, y, width, height;
static HBITMAP bitmap;
static HGDIOBJ previous;

EXTERN_C SNUGGLING_API HRESULT Initialize(HWND window, int width, int height) {
	// Configure file-level variables.
	if(::window) {
		Complete();
	}
	::window= window;
	::width= width;
	::height= height;

	// Create the device contexts for the Pinball Arcade window.
	sourceContext= GetDC(window);
	targetContext= CreateCompatibleDC(sourceContext);

	// Create and select the bitmap to hold the table's banner image.
	int bitCount= GetDeviceCaps(sourceContext, BITSPIXEL);
	size= bitCount * width * height / 8;
	BITMAPINFO info;
	info.bmiHeader.biSize= sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth= width;
	info.bmiHeader.biHeight= height;
	info.bmiHeader.biPlanes= 1;
	info.bmiHeader.biBitCount = static_cast<WORD>(bitCount);
	info.bmiHeader.biCompression= BI_RGB;
	bitmap= CreateDIBSection(targetContext, &info, DIB_RGB_COLORS, &data, 0, 0);
	previous= SelectObject(targetContext, bitmap);
	return S_OK;
}

EXTERN_C SNUGGLING_API HRESULT GetTableId(int x, int y, table_id_t* rv) {
	// Copy the table's banner image into the memory context.
	if(BitBlt(targetContext, 0, 0, width, height, sourceContext, x, y, SRCCOPY)) {
#ifdef _DEBUG
		static std::vector<BYTE> lastData;
		static unsigned smallest= ~0u;
		if(lastData.empty()) {
			lastData.assign((BYTE*)data, (BYTE*)data + size);
		} else {
			auto* p= (BYTE*)data;
			for(unsigned i= 0; i < size; ++i) {
				if(p[i] != lastData[i] && i < smallest) {
					smallest= i;
					TCHAR sz[16];
					OutputDebugString(_T("buffers differ at "));
					wsprintf(sz, _T("%d"), i);
					OutputDebugString(sz);
					OutputDebugString(_T("\n"));
					break;
				}
			}
		}
#endif

		// Hash the image data into an identifier and return it.
		*rv= HashData(data, size);
		return S_OK;
	}

	// An error occurred copying the image.  This happens usually because the
	// Pinball Arcade window closed.
	return E_FAIL;
}

EXTERN_C SNUGGLING_API HRESULT Complete() {
	// Clean up all graphical objects.
	SelectObject(targetContext, previous);
	DeleteObject(bitmap);
	DeleteDC(targetContext);
	ReleaseDC(window, sourceContext);
	return S_OK;
}
