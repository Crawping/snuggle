#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SNUGGLING_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SNUGGLING_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SNUGGLING_EXPORTS
# define SNUGGLING_API __declspec(dllexport)
#else
# define SNUGGLING_API __declspec(dllimport)
#endif

using table_id_t= unsigned long long;

table_id_t HashData(void* data, unsigned size);

EXTERN_C SNUGGLING_API HRESULT Initialize(HWND window, int width, int height);
EXTERN_C SNUGGLING_API HRESULT GetTableId(int x, int y, table_id_t* rv);
EXTERN_C SNUGGLING_API HRESULT Complete();
