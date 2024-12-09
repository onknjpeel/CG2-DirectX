#pragma once
#include "MathEx.h"
#include "wrl.h"
#include <d3d12.h>

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
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

class SpriteCommon;

class Sprite
{
public:

	void Initialize(SpriteCommon* spriteCommon);

	void Update();

	void Draw();

	const Vector2& GetPosition()const { return position; }
	void SetPosition(const Vector2& position) { this->position = position; }

	float GetRotation()const { return rotation; }
	void SetRotation(float rotation) { this->rotation = rotation; }

	const Vector4& GetColor()const { return materialData->color; }
	void SetColor(const Vector4& color) { materialData->color = color; }

	const Vector2& GetSize()const { return size; }
	void SetSize(const Vector2& size) { this->size = size; }

private:

	void CreateVertexData();

	void CreateIndexData();

	void CreateMaterialData();

	void CreateTransformData();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource = nullptr;

	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	Material* materialData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	Vector2 position = { 0.0f,0.0f };

	float rotation = 0.0f;

	Vector2 size = { 640.0f,360.0f };

	SpriteCommon* spriteCommon = nullptr;
};

