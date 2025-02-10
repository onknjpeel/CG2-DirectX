#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include "DXCommon.h"

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4x4 {
	float m[4][4];
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct MaterialData {
	std::string textureFilePath;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

class IModel
{
public:
	virtual void Init(DXCommon* pointer) = 0;
	virtual void LoadModel() = 0;
	virtual void CreateModel() = 0;
	virtual void SetModel() = 0;
	virtual void DrawModel() = 0;
	virtual void SetTransformMatData(TransformationMatrix* matData) = 0;

protected:
#pragma region マテリアルデータを読む関数
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
#pragma endregion

#pragma region モデルデータを読む関数
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
#pragma endregion

	Matrix4x4 MakeIdentity4x4() {
		Matrix4x4 result = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};
		return result;
	}

#pragma region 長さ
	float Length(const Vector3& v) {
		float result;
		result = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		return result;
	}
#pragma endregion

#pragma region 正規化
	Vector3 Normalize(const Vector3& v) {
		Vector3 result;
		result.x = v.x / Length(v);
		result.y = v.y / Length(v);
		result.z = v.z / Length(v);
		return result;
	}

protected:
	ModelData modelData;

	DXCommon* dxCommon = nullptr;
	DirectX::ScratchImage mipImages;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	uint32_t kSubdivision = 30;

	uint32_t startIndex = kSubdivision * kSubdivision * 6;

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	DirectionalLight* directionalLightData = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource;
	Material* materialData = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	TransformationMatrix* transformationMatrixData = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	VertexData* vertexData = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	uint32_t* indexData = nullptr;

};