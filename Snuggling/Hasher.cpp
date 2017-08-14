#include "stdafx.h"
#include "Snuggling.h"

namespace {
	struct HashValue {
		table_id_t a, b;

		BYTE* operator&() { return reinterpret_cast<BYTE*>(this); }
		operator table_id_t() { return a ^ b; }
	};

	static_assert(sizeof(HashValue) == 16, "invalid hash value size"); // 128 bits
}

table_id_t HashData(void* data, unsigned size) {
	HashValue hashValue{};
	HCRYPTPROV provider;
	if(CryptAcquireContext(&provider, nullptr, nullptr, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		HCRYPTHASH hash;
		if(CryptCreateHash(provider, CALG_MD5, 0, 0, &hash)) {
			if(CryptHashData(hash, reinterpret_cast<BYTE*>(data), size, 0)) {
				auto hashSize= static_cast<DWORD>(sizeof(hashValue));
				CryptGetHashParam(hash, HP_HASHVAL, &hashValue, &hashSize, 0);
			}
			CryptDestroyHash(hash);
		}
		CryptReleaseContext(provider, 0);
	}
	return hashValue;
}
