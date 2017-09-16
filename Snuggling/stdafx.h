#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

// Windows header files
#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>
#include <wrl.h>
#include <comdef.h>

// DirectX header files
#include <d3d11_2.h>
#include <dxgi1_3.h>

// Standard library header files
#undef max
#undef min
#include <algorithm>
#include <memory>
