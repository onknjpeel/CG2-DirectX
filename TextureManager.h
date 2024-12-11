#pragma once
#include <string>
#include <d3d12.h>
#include <wrl.h>
#include "externals/DirectXTex/DirectXTex.h"

class TextureManager
{
public:
	static TextureManager* GetInstance();

	void Finalize();

	void Initialize();

	void LoadTexture(const std::string& firePath);

private:
	struct TextureData {
		std::string filepath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

	std::vector<TextureData> textureDatas;

	static const uint32_t kMaxSRVCount;
};

