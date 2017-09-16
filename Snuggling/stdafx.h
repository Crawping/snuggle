#pragma once

#include "targetver.h"

#ifdef _DEBUG
#	ifndef DEBUG
#		define DEBUG
#	endif
#elif defined(DEBUG)
#	error inconsistent DEBUG directives
#elif !defined(NDEBUG)
#	define NDEBUG
#endif
#ifndef STRICT
#	define STRICT
#endif

#pragma warning(disable: 4459) // declaration of '' hides global declaration

// Windows include directives
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>
#include <d3d11_2.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include <comdef.h>

// Standard library include directives
#undef max
#undef min
#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#ifdef _UNICODE
typedef std::wifstream tifstream;
typedef std::wofstream tofstream;
typedef std::wostream tostream;
typedef std::wstring tstring;
typedef std::wstringstream tstringstream;
#	define tout std::wcout
#	define tin std::wcin
#else
typedef std::ifstream tifstream;
typedef std::ofstream tofstream;
typedef std::ostream tostream;
typedef std::string tstring;
typedef std::stringstream tstringstream;
#	define tout std::cout
#	define tin std::cin
#endif
