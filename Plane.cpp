#include "Plane.h"
#include "externals/imgui/imgui.h"

void Plane::LoadModel()
{
	modelData = LoadObjFile("resources", "plane.obj");

	mipImages = dxCommon->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	textureResource = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata);
	intermediate = dxCommon->UploadTextureData(textureResource, mipImages);
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
	textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);

	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
}

void Plane::CreateModel()
{
	directionalLightResource = dxCommon->CreateBufferResource(sizeof(DirectionalLight));

	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	*directionalLightData = DirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f,-1.0f,0.0f }, 1.0f);
#pragma region VertexResourceを生成
	vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
#pragma endregion

#pragma region Material用のResourceを作る
	materialResource = dxCommon->CreateBufferResource(sizeof(Material));

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();

#pragma endregion

#pragma region TransformationMatrix用のResourceを作る
	transformationMatrixResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	transformationMatrixData->WVP = MakeIdentity4x4();
#pragma endregion

#pragma region VertexBufferViewを作成
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());

	vertexBufferView.StrideInBytes = sizeof(VertexData);
#pragma endregion

#pragma region Resourceにデータを書き込む(頂点データの更新)

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

#pragma endregion

#pragma region IndexResource
	indexResource = dxCommon->CreateBufferResource(sizeof(uint32_t) * kSubdivision * kSubdivision * 6);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();

	indexBufferView.SizeInBytes = sizeof(uint32_t) * kSubdivision * kSubdivision * 6;

	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma region IndexResourceに書き込み
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	for (uint32_t i = 0; i < kSubdivision; ++i) {
		for (uint32_t j = 0; j < kSubdivision; ++j) {
			uint32_t start = (i * kSubdivision + j) * 6;
			uint32_t a = i * (kSubdivision + 1) + j;
			indexData[start] = a; indexData[start + 1] = a + kSubdivision + 1; indexData[start + 2] = a + 1;
			indexData[start + 3] = a + kSubdivision + 1; indexData[start + 4] = a + kSubdivision + 2; indexData[start + 5] = a + 1;
		}
	}

#pragma endregion
}

void Plane::SetModel()
{
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());

	dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
}

void Plane::DrawModel()
{
	if (isDraw) {
		ImGui::Begin("plane");
		if (ImGui::TreeNode("DirectionalLight")) {
			ImGui::DragFloat3("direction", &directionalLightData->direction.x, 0.01f);
			directionalLightData->direction = Normalize(directionalLightData->direction);
			ImGui::DragFloat("intensity", &directionalLightData->intensity, 0.01f);
			ImGui::TreePop();
		}
		ImGui::End();

		dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	}
}