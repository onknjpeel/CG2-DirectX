#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include <array>
#include <dxcapi.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "StringUtility.h"

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

	void PreDraw();

	void PostDraw();

	ID3D12Device* GetDevice()const { return device.Get(); }

	ID3D12GraphicsCommandList* GetCommandList()const { return commandList.Get(); }

	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile
	);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

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

	   D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	   D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	   Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	   uint64_t fenceValue = 0;
	   HANDLE fenceEvent;

	   std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	   D3D12_VIEWPORT viewport{};
	   D3D12_RECT scissorRect{};

	   IDxcUtils* dxcUtils = nullptr;
	   IDxcCompiler3* dxcCompiler = nullptr;
	   IDxcIncludeHandler* includeHandler = nullptr;

	   D3D12_RESOURCE_BARRIER barrier{};

	   WinApp* winApp = nullptr;
};