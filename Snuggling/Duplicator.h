#pragma once

class Duplicator {
public:
	Duplicator();
	~Duplicator();
	table_id_t Doit(int x, int y, int width, int height);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<IDXGIDevice1> pDevice1;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter1;
	Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;
	Microsoft::WRL::ComPtr<IDXGIOutput1> pOutput1;
	Microsoft::WRL::ComPtr<IDXGIOutputDuplication> pDuplication;
};
