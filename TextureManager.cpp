#include "TextureManager.h"
#include "StringUtility.h"

using namespace StringUtility;

TextureManager* TextureManager::instance = nullptr;

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DXCommon* dxCommon)
{
	this->dxCommon = dxCommon;
	textureDatas.reserve(DXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filepath == filePath; }
	);
	if (it != textureDatas.end()) {
		return;
	}

	assert(textureDatas.size() + kSRVIndexTop < DXCommon::kMaxSRVCount);

	HRESULT hr;

	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	textureDatas.resize(textureDatas.size() + 1);

	TextureData& textureData = textureDatas.back();

	textureData.filepath = filePath;
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon->CreateTextureResource(dxCommon->GetDevice(), textureData.metadata);
	ComPtr<ID3D12Resource> intermediate = dxCommon->UploadTextureData(textureData.resource, mipImages);

	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

	textureData.srvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(srvIndex);

	dxCommon->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {
			return textureData.filepath == filePath;
		}
	);

	if (it != textureDatas.end()) {
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{
	assert(textureIndex < DXCommon::kMaxSRVCount);

	TextureData& textureData = textureDatas[textureIndex];
	return textureData.srvHandleGPU;
}