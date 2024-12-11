#include "TextureManager.h"
#include "StringUtility.h"
using namespace StringUtility;

TextureManager* TextureManager::instance = nullptr;

const uint32_t DXCommon::kMaxSRVCount = 512;

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

void TextureManager::Initialize()
{
	textureDatas.reserve(DXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	textureDatas.resize(textureDatas.size() + 1);

	TextureData& textureData = textureDatas.back();

	textureData.filepath = filePath;
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource= dxCommon->CreateTextureResource(dxCommon->GetDevice(), textureData.metadata);

	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1);

	textureData.srvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(srvIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
}
