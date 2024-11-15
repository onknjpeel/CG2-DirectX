#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include <array>
#include <dxcapi.h>

class DXCommon
{
public:
	void Initialize(WinApp* winApp);

	void InitDevice();

	void InitCommand();

	void MakeSwapChain();

	void MakeDepthBuffer();

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible
	);

	void MakeDescriptorHeap();

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	void InitRTV();

	void InitDSV();

	void CreateFence();

	void MakeViewport();

	void MakeScissorRect();

	void MakeDXCCompiler();
	
	void InitImGui();

private:;
	   Microsoft::WRL::ComPtr<ID3D12Device> device;

	   Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	   Microsoft::WRL::ComPtr <ID3D12CommandAllocator> commandAllocator;
	   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	   Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;

	   D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	   Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;

	   DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	   ID3D12Resource* resource = nullptr;

	   uint32_t descriptorSizeSRV;
	   uint32_t descriptorSizeRTV;
	   uint32_t descriptorSizeDSV;

	   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	   Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	   D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	   std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	   D3D12_VIEWPORT viewport{};
	   D3D12_RECT scissorRect{};

	   IDxcUtils* dxcUtils = nullptr;
	   IDxcCompiler3* dxcCompiler = nullptr;
	   IDxcIncludeHandler* includeHandler = nullptr;

	   WinApp* winApp = nullptr;
};