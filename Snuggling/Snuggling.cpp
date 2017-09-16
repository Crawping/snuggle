#include "stdafx.h"
#include "Snuggling.h"
#include "Duplicator.h"

static std::unique_ptr<Duplicator> duplicator;

EXTERN_C SNUGGLING_API HRESULT Initialize() {
	if(duplicator) {
		Complete();
	}
	duplicator= std::make_unique<Duplicator>();
	return S_OK;
}

EXTERN_C SNUGGLING_API HRESULT GetTableId(int x, int y, int width, int height, table_id_t* rv) {
	try {
		*rv= duplicator->Doit(x, y, width, height);
		return S_OK;
	} catch(_com_error const& ex) {
		return ex.Error();
	}
}

EXTERN_C SNUGGLING_API HRESULT Complete() {
	duplicator.reset();
	return S_OK;
}
