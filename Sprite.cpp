#include "Sprite.h"
#include "SpriteCommon.h"

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon = spriteCommon;

	CreateVertexData();
	CreateIndexData();
	CreateMaterialData();
	CreateTransformData();
}

void Sprite::Update()
{
	vertexData[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData[0].texcoord = { 0.0f,0.0f };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };

	vertexData[1].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexData[1].texcoord = { 1.0f,0.0f };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };

	vertexData[2].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexData[2].texcoord = { 0.0f,1.0f };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };

	vertexData[3].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexData[3].texcoord = { 1.0f,1.0f };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };

	indexData[0] = 0; indexData[1] = 1; indexData[2] = 2;
	indexData[3] = 1; indexData[4] = 3; indexData[5] = 2;

	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f } };

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, WinApp::kClientWidth, WinApp::kClientHeight, 0.0f, 100.0f);
	Matrix4x4 worldProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	*transformationMatrixData = { worldProjectionMatrix,worldMatrix };
}

void Sprite::Draw()
{
	spriteCommon->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	spriteCommon->GetDxCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	spriteCommon->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, spriteCommon->GetDxCommon()->GetSRVGPUDescriptorHandle(2));

	spriteCommon->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateVertexData()
{
	vertexResource = spriteCommon->GetDxCommon()->CreateBufferResource(UINT(sizeof(VertexData) * 6));

	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * 6);
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
}

void Sprite::CreateIndexData()
{
	indexResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(uint32_t) * 6);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
}

void Sprite::CreateMaterialData()
{
	materialResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(Material));

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();
}

void Sprite::CreateTransformData()
{
	transformationMatrixResource = spriteCommon->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}
