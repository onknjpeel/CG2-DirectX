#include <dxgidebug.h>
#include <vector>
#include "externals/DirectXTex/DirectXTex.h"
#include <fstream>
#include <sstream>
#include "Input.h"
#include "WinApp.h"
#include "DXCommon.h"
#include "Logger.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "D3DResourceLeakChecker.h"
#include "Sprite.h"
#include "SpriteCommon.h"

#pragma comment(lib,"dxcompiler.lib")

#pragma region 構造体群

#pragma region 平行光源
struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};
#pragma endregion

#pragma region マテリアルデータ
struct MaterialData {
	std::string textureFilePath;
};
#pragma endregion

#pragma region モデルデータ
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};
#pragma endregion
#pragma endregion

#pragma region 変数群
#pragma region UVTransformの変数
Transform uvTransformSprite{
	{1.0f,1.0f,1.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
};
#pragma endregion

#pragma region 切り替え用変数
bool useMonsterBall = true;
#pragma endregion
#pragma region Transform変数
Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f } };
#pragma endregion

#pragma region TransformSprite
Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
#pragma endregion

#pragma region TriangleのTlansform
Transform transformTriangle{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f } };
#pragma endregion

#pragma region cameraTransform変数
Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };
#pragma endregion
#pragma endregion

#pragma region 関数群
#pragma region 行列に関する関数

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
#pragma endregion

#pragma endregion

#pragma region DepthFunc
bool DepthFunc(float currZ, float prevZ) {
	return currZ <= prevZ;
}
#pragma endregion

#pragma region マテリアルデータを読む関数
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
#pragma region 必要な変数の宣言
	MaterialData materialData;
	std::string line;
#pragma endregion

#pragma region ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
#pragma endregion

#pragma region マテリアルデータを構築
	while (std::getline(file, line)) {
		std::string identifer;
		std::istringstream s(line);
		s >> identifer;

		if (identifer == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
#pragma endregion
	return materialData;
}
#pragma endregion

#pragma region モデルデータを読む関数
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
#pragma region 中で必要となる変数の宣言
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;
#pragma endregion

#pragma region ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
#pragma endregion

#pragma region 実際にファイルを読みモデルデータを構築
	while (std::getline(file, line)) {
		std::string identifer;
		std::istringstream s(line);
		s >> identifer;
#pragma region 頂点情報を読む
		if (identifer == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifer == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifer == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		}
#pragma region 三角形を作る
		else if (identifer == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
#pragma endregion
#pragma region マテリアルを読み込み
		else if (identifer == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
#pragma endregion

#pragma endregion
	}
#pragma endregion
	return modelData;
}
#pragma endregion

#pragma endregion

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker* leakChecker = nullptr;

	WinApp* winApp = nullptr;

	winApp = new WinApp();
	winApp->Initialize();

	DXCommon* dxCommon = nullptr;

	dxCommon = new DXCommon();
	dxCommon->Initialize(winApp);

	SpriteCommon spriteCommon;
	spriteCommon.Initialize(dxCommon);

#ifdef _DEBUG

#pragma region DebugLayer
	Microsoft::WRL::ComPtr <ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#pragma endregion

#endif

#pragma region DXGIFactoryの生成
	Microsoft::WRL::ComPtr <IDXGIFactory7> dxgiFactory = nullptr;

	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	assert(SUCCEEDED(hr));
#pragma endregion

	Input* input = nullptr;

	input = new Input();
	input->Initialize(winApp);

	/*Sprite* sprite = new Sprite();
	sprite->Initialize(&spriteCommon);
*/
	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 5; ++i) {
		Sprite* sprite = new Sprite();
		sprite->Initialize(&spriteCommon);
		sprites.push_back(sprite);
	}

#ifdef _DEBUG

#pragma region エラー・警告時に停止
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;

	if (SUCCEEDED(dxCommon->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

#pragma region エラーと警告の抑制
		D3D12_MESSAGE_ID denyIds[] = { D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };

		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;

		infoQueue->PushStorageFilter(&filter);
#pragma endregion

	}
#pragma endregion

#endif

#pragma region RootSignatureを生成//
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

#pragma region RootParameter

	D3D12_ROOT_PARAMETER rootParameter[4] = {};

#pragma region DescriptorRange

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[0].Descriptor.ShaderRegister = 0;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].Descriptor.ShaderRegister = 0;

#pragma region DescriptorTable

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

#pragma endregion

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].Descriptor.ShaderRegister = 1;

	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = _countof(rootParameter);

#pragma endregion

#pragma region Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};

	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
#pragma endregion
	/*
		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
		if (FAILED(hr)) {
			Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
			assert(false);
		}

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
		hr = dxCommon->GetDevice()->CreateRootSignature(0,
			signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature)
		);
		assert(SUCCEEDED(hr));
		*/
#pragma endregion

#pragma region InputLayoutの設定(拡張)
		/*
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[1].SemanticName = "TEXCOORD";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs[2].SemanticName = "NORMAL";
		inputElementDescs[2].SemanticIndex = 0;
		inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementDescs;
		inputLayoutDesc.NumElements = _countof(inputElementDescs);
	*/
#pragma endregion

#pragma region BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
#pragma endregion

#pragma region RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
#pragma endregion

#pragma region ShaderをCompileする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
#pragma endregion

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = dxCommon->CreateBufferResource(sizeof(DirectionalLight));

	DirectionalLight* directionalLightData = nullptr;

	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	*directionalLightData = DirectionalLight({ 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f,-1.0f,0.0f }, 1.0f);

#pragma region 球作成用変数宣言
	uint32_t kSubdivision = 30;

	uint32_t startIndex = kSubdivision * kSubdivision * 6;
#pragma endregion

#pragma region モデル読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");
#pragma endregion

#pragma region 2枚目のtextureを読む
	DirectX::ScratchImage mipImages2 = dxCommon->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate2 = dxCommon->UploadTextureData(textureResource2, mipImages2);
#pragma endregion

#pragma region Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(dxCommon->GetDevice(), metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediate = dxCommon->UploadTextureData(textureResource, mipImages);
#pragma endregion

#pragma region SRVを作る
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);

	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

#pragma endregion

#pragma region SRV2つ目
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata.mipLevels);

	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetSRVCPUDescriptorHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetSRVGPUDescriptorHandle(2);

	dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
#pragma endregion

#pragma region VertexResourceを生成
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
#pragma endregion

#pragma region Material用のResourceを作る
	/*Microsoft::WRL::ComPtr <ID3D12Resource> materialResource = dxCommon->CreateBufferResource(sizeof(Material));

	Material* materialData = nullptr;

	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
*/
#pragma endregion

#pragma region TransformationMatrix用のResourceを作る
/*Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

TransformationMatrix* transformationMatrixData = nullptr;

transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

transformationMatrixData->WVP = MakeIdentity4x4();*/
#pragma endregion

#pragma region VertexBufferViewを作成
/*D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();

vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());

vertexBufferView.StrideInBytes = sizeof(VertexData);*/
#pragma endregion

#pragma region Resourceにデータを書き込む(頂点データの更新)
/*VertexData* vertexData = nullptr;

vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
*/
#pragma endregion

#pragma region IndexResource
/*Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = dxCommon->CreateBufferResource(sizeof(uint32_t) * kSubdivision * kSubdivision * 6);

D3D12_INDEX_BUFFER_VIEW indexBufferView{};

indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();

indexBufferView.SizeInBytes = sizeof(uint32_t) * kSubdivision * kSubdivision * 6;

indexBufferView.Format = DXGI_FORMAT_R32_UINT;*/
#pragma endregion

#pragma region IndexResourceに書き込み
/*uint32_t* indexData = nullptr;
indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
for (uint32_t i = 0; i < kSubdivision; ++i) {
	for (uint32_t j = 0; j < kSubdivision; ++j) {
		uint32_t start = (i * kSubdivision + j) * 6;
		uint32_t a = i * (kSubdivision + 1) + j;
		indexData[start] = a; indexData[start + 1] = a + kSubdivision + 1; indexData[start + 2] = a + 1;
		indexData[start + 3] = a + kSubdivision + 1; indexData[start + 4] = a + kSubdivision + 2; indexData[start + 5] = a + 1;
	}
}
*/
#pragma endregion

#pragma region 三角形二枚用のVertexResource
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceTriangle = dxCommon->CreateBufferResource(sizeof(VertexData) * 6);
#pragma endregion

#pragma region TransformationMatrix用のResourceを作る(三角形)
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceTriangle = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

	TransformationMatrix* transformationMatrixDataTriangle = nullptr;

	transformationMatrixResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataTriangle));

	transformationMatrixDataTriangle->WVP = MakeIdentity4x4();
#pragma endregion

#pragma region VertexBufferViewTriangleを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewTriangle{};

	vertexBufferViewTriangle.BufferLocation = vertexResourceTriangle->GetGPUVirtualAddress();

	vertexBufferViewTriangle.SizeInBytes = sizeof(VertexData) * 6;

	vertexBufferViewTriangle.StrideInBytes = sizeof(VertexData);
#pragma endregion

#pragma region 三角形の頂点データ
	VertexData* vertexDataTriangle = nullptr;
	vertexResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataTriangle));

	vertexDataTriangle[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexDataTriangle[0].texcoord = { 0.0f,1.0f };
	vertexDataTriangle[0].normal = { 0.0f,0.0f,-1.0f };
	vertexDataTriangle[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexDataTriangle[1].texcoord = { 0.5f,0.0f };
	vertexDataTriangle[1].normal = { 0.0f,0.0f,-1.0f };
	vertexDataTriangle[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexDataTriangle[2].texcoord = { 1.0f,1.0f };
	vertexDataTriangle[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataTriangle[3].position = { -0.5f,-0.5f,0.5f,1.0f };
	vertexDataTriangle[3].texcoord = { 0.0f,1.0f };
	vertexDataTriangle[3].normal = { 0.0f,0.0f,-1.0f };
	vertexDataTriangle[4].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataTriangle[4].texcoord = { 0.5f,0.0f };
	vertexDataTriangle[4].normal = { 0.0f,0.0f,-1.0f };
	vertexDataTriangle[5].position = { 0.5f,-0.5f,-0.5f,1.0f };
	vertexDataTriangle[5].texcoord = { 1.0f,1.0f };
	vertexDataTriangle[5].normal = { 0.0f,0.0f,-1.0f };
#pragma endregion

#pragma region IndexResourceTriangle
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexResourceTriangle = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewTriangle{};

	indexBufferViewTriangle.BufferLocation = IndexResourceTriangle->GetGPUVirtualAddress();

	indexBufferViewTriangle.SizeInBytes = sizeof(uint32_t) * 6;

	indexBufferViewTriangle.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma region IndexResourceTiangleに書き込み
	uint32_t* indexDataTriangle = nullptr;
	IndexResourceTriangle->Map(0, nullptr, reinterpret_cast<void**>(&indexDataTriangle));

	indexDataTriangle[0] = 0; indexDataTriangle[1] = 1; indexDataTriangle[2] = 2;
	indexDataTriangle[3] = 3; indexDataTriangle[4] = 4; indexDataTriangle[5] = 5;
#pragma endregion

#pragma region VertexResourceSpriteを生成
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(sizeof(VertexData) * 4);
#pragma endregion

#pragma region Material用のResourceを作る(sprite)
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite = dxCommon->CreateBufferResource(sizeof(Material));

	Material* materialDataSprite = nullptr;

	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

	materialDataSprite->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = MakeIdentity4x4();
#pragma endregion

#pragma region TransformationMatrix用のResourceを作る(Sprite)
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = dxCommon->CreateBufferResource(sizeof(TransformationMatrix));

	TransformationMatrix* transformationMatrixDataSprite = nullptr;

	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));

	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
#pragma endregion

#pragma region VertexBufferViewSpriteを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();

	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 4;

	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
#pragma endregion

#pragma region スプライトの頂点データ
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));


	vertexDataSprite[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[0].texcoord = { 0.0f,0.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[1].position = { 640.0f,0.0f,0.0f,1.0f };
	vertexDataSprite[1].texcoord = { 1.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[2].position = { 0.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[2].texcoord = { 0.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f,-1.0f };

	vertexDataSprite[3].position = { 640.0f,360.0f,0.0f,1.0f };
	vertexDataSprite[3].texcoord = { 1.0f,1.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f,-1.0f };
#pragma endregion

#pragma region IndexResourceSprite
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);

	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();

	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;

	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
#pragma endregion

#pragma region IndexResourceSpriteに書き込み
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;
#pragma endregion

#pragma region CreateDepthStencilextureResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource = dxCommon->CreateDepthStencilTextureResource(WinApp::kClientWidth, WinApp::kClientHeight);
#pragma endregion

#pragma region DSVを構築する
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap = dxCommon->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	dxCommon->GetDevice()->CreateDepthStencilView(
		depthStencilResource.Get(),
		&dsvDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
	);
#pragma endregion


#pragma region DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	depthStencilDesc.DepthEnable = true;

	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
#pragma endregion

#pragma region PSOを生成する//
	/*
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
*/
#pragma endregion

//文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	MSG msg{};

	while (msg.message != WM_QUIT) {
		if (winApp->ProcessMessage()) {
			break;
		}
		else {
			//ゲームの処理
			input->Update();

			if (input->TriggerKey(DIK_0)) {
				OutputDebugStringA("Hit 0\n");
			}


#pragma region フレームが始まる旨を告げる
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#pragma endregion

#pragma region Transformを使ってCBufferを更新する
			/*
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
			Matrix4x4 worldProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			*transformationMatrixData = { worldProjectionMatrix,worldMatrix };
*//*
			Vector2 position = sprite->GetPosition();

			//position.x += 0.1f;
			//position.y += 0.1f;

			sprite->SetPosition(position);

			float rotation = sprite->GetRotation();

			//rotation += 0.01f;

			sprite->SetRotation(rotation);

			Vector4 color = sprite->GetColor();
			color.x += 0.01f;
			if (color.x > 1.0f) {
				color.x -= 1.0f;
			}
			sprite->SetColor(color);

			Vector2 size = sprite->GetSize();
			size.x += 0.1f;
			size.y += 0.1f;
			sprite->SetSize(size);

			//sprite->Update();

*/
			for (size_t i = 0; i < sprites.size(); ++i) {
				Sprite* sprite = sprites[i];

				Vector2 position = sprite->GetPosition();

				float rotation = sprite->GetRotation();

				Vector4 color = sprite->GetColor();

				Vector2 size = sprite->GetSize();

				size.x = 320.0f;
				size.y = 160.0f;

				if (i == 0) {
					position.x = 0.0f;
					position.y = 0.0f;

					//rotation += 0.01f;

				}
				else if (i == 1) {
					position.x = 640.0f;
					position.y = 0.0f;

					//rotation += 0.01f;
				}
				else if (i == 2) {
					position.x = 320.0f;
					position.y = 180.0f;

					//rotation += 0.01f;
				}
				else if (i == 3) {
					position.x = 0.0f;
					position.y = 320.0f;

					//rotation += 0.01f;
				}
				else if (i == 4) {
					position.x = 640.0f;
					position.y = 320.0f;

					//rotation += 0.01f;
				}

				sprite->SetPosition(position);

				sprite->SetRotation(rotation);

				color.x += 0.01f;
				if (color.x > 1.0f) {
					color.x -= 1.0f;
				}
				sprite->SetColor(color);

				sprite->SetSize(size);

				sprite->Update();
			}

#pragma endregion

#pragma region WVPMatrixを作って書き込む//sprite
			/*
				Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
				Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
				Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
				Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
				*transformationMatrixDataSprite = { worldViewProjectionMatrixSprite, worldMatrixSprite };
			*/
#pragma endregion

#pragma region Transformを使って書き込む//三角形二枚
			/*
			Matrix4x4 worldMatrixTriangle = MakeAffineMatrix(transformTriangle.scale, transformTriangle.rotate, transformTriangle.translate);
			Matrix4x4 cameraMatrixTriangle = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrixTriangle = Inverse(cameraMatrixTriangle);
			Matrix4x4 projectionMatrixTriangle = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
			Matrix4x4 worldProjectionMatrixTriangle = Multiply(worldMatrixTriangle, Multiply(viewMatrixTriangle, projectionMatrixTriangle));
			*transformationMatrixDataTriangle = { worldProjectionMatrixTriangle ,worldMatrixTriangle };
			*/
#pragma endregion

#pragma region UVTransform行列
			/*
			Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			materialDataSprite->uvTransform = uvTransformMatrix;
			*/
#pragma endregion

#pragma region コマンドを積み込み確定させる

#pragma region ImGuiの処理
			ImGui::ShowDemoWindow();
#pragma endregion

			ImGui::Begin("Window");
			if (ImGui::TreeNode("model")) {
				ImGui::DragFloat3("Model.translate", &transform.translate.x, 0.01f);
				ImGui::DragFloat3("Model.rotate", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("Model.scale", &transform.scale.x, 0.01f);
				ImGui::Checkbox("useMonsterBall", &useMonsterBall);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("DirectionalLight")) {
				ImGui::DragFloat3("direction", &directionalLightData->direction.x, 0.01f);
				directionalLightData->direction = Normalize(directionalLightData->direction);
				ImGui::DragFloat("intensity", &directionalLightData->intensity, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("UV")) {
				ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
				ImGui::TreePop();
			}
			ImGui::Text("Sprite");
			ImGui::DragFloat3("Sprite.position", &transformSprite.translate.x, 0.25f);
			ImGui::End();

#pragma region ImGuiの内部コマンドを生成
			ImGui::Render();
#pragma endregion

#pragma region コマンドを積む
			dxCommon->PreDraw();

			spriteCommon.DrawSpriteCommon();

			for (Sprite* sprite : sprites) {
				sprite->Draw();
			}

			/*
			dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
			dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());

			dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferView);

			dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			*/

#pragma region CBVを設定する
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
#pragma endregion

#pragma region DescriptorTableを設定する
			//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
#pragma endregion

			//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

#pragma region 三角形二枚描画
			/*
			dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewTriangle);
			dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewTriangle);
			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceTriangle->GetGPUVirtualAddress());
			dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			//dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
*/
#pragma endregion

#pragma region Sprite描画

/*dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
//dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
*/
#pragma endregion

#pragma region ImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());
#pragma endregion

#pragma endregion

			dxCommon->PostDraw();

		}
	}

#pragma region ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#pragma endregion

	std::string str0{ "STRING!!!" };

	std::string str1{ std::to_string(10) };

#pragma region 解放処理
	for (Sprite* sprite : sprites) {
		delete sprite;
	}
	delete input;
	winApp->Finalize();
	delete winApp;
	winApp = nullptr;
	delete dxCommon;
	//CloseHandle(fenceEvent);
#pragma endregion

#pragma region ReportLiveObjects
	delete leakChecker;
#pragma endregion

	return 0;
}

//GEブランチ