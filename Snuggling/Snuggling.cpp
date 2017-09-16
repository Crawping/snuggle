#include "stdafx.h"
#include "Snuggling.h"
#include "Duplicator.h"

static std::unique_ptr<Duplicator> duplicator;

EXTERN_C SNUGGLING_API HRESULT Initialize() {
	OutputDebugString(_T("Initialize\n"));
	//DebugBreak();
	if(duplicator) {
		Complete();
	}
	duplicator= std::make_unique<Duplicator>();
	return S_OK;
}

EXTERN_C SNUGGLING_API HRESULT GetTableId(int y, int width, int height, table_id_t* rv) {
#ifdef _DEBUG
	TCHAR sz[999];
	wsprintf(sz, _T("y %d, w %d, h %d\n"), y, width, height);
	OutputDebugString(sz);
	//DebugBreak();
#endif
	try {
		*rv= duplicator->Doit(y, width, height);
		return S_OK;
	} catch(_com_error const& ex) {
		return ex.Error();
	}
}

EXTERN_C SNUGGLING_API HRESULT Complete() {
	OutputDebugString(_T("Complete\n"));
	//DebugBreak();
	duplicator.reset();
	return S_OK;
}
