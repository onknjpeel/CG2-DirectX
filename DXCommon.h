#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"
#include <array>
#include <dxcapi.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "StringUtility.h"
#include <chrono>

using namespace Microsoft::WRL;

class DXCommon
{
public:
	void Initialize(WinApp* winApp);

	void InitDevice();

	void InitCommand();

	void MakeSwapChain();

	void MakeDepthBuffer();

	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
		bool shaderVisible
	);

	void MakeDescriptorHeap();

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);

	void InitRTV();

	void InitDSV();

	void CreateFence();

	void MakeViewport();

	void MakeScissorRect();

	void MakeDXCCompiler();

	void InitImGui();

	void PreDraw();

	void PostDraw();

	ComPtr<ID3D12Device> GetDevice()const { return device.Get(); }

	ComPtr<ID3D12GraphicsCommandList> GetCommandList()const { return commandList.Get(); }

	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile
	);

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	ComPtr<ID3D12Resource> CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata);

	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(int32_t width, int32_t height);

	ComPtr<ID3D12Resource> UploadTextureData(ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);

public:
	static const uint32_t kMaxSRVCount;

private:
	void InitializeFixFPS();

	void UpdateFixFPS();

private:
	   ComPtr<ID3D12Device> device;

	   ComPtr<IDXGIFactory7> dxgiFactory;

	   ComPtr <ID3D12CommandAllocator> commandAllocator;
	   ComPtr<ID3D12GraphicsCommandList> commandList;
	   ComPtr<ID3D12CommandQueue> commandQueue;

	   D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};

	   ComPtr<IDXGISwapChain4> swapChain;

	   DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};

	   ComPtr<ID3D12Resource> resource = nullptr;

	   uint32_t descriptorSizeSRV;
	   uint32_t descriptorSizeRTV;
	   uint32_t descriptorSizeDSV;

	   ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	   ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	   ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	   D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	   D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;

	   D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};

	   ComPtr<ID3D12Fence> fence = nullptr;
	   uint64_t fenceValue = 0;
	   HANDLE fenceEvent;

	   std::array<ComPtr<ID3D12Resource>, 2> swapChainResources;

	   ComPtr<ID3D12Resource> depthStencilResource;

	   D3D12_VIEWPORT viewport{};
	   D3D12_RECT scissorRect{};

	   ComPtr<IDxcUtils> dxcUtils = nullptr;
	   ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	   ComPtr<IDxcIncludeHandler> includeHandler = nullptr;

	   D3D12_RESOURCE_BARRIER barrier{};

	   std::chrono::steady_clock::time_point reference_;

	   WinApp* winApp = nullptr;
};