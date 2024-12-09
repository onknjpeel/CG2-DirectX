#pragma once
#include "externals/DirectXTex/d3dx12.h"
#include "DXCommon.h"

class SpriteCommon
{
public:
	void Initialize(DXCommon* dxCommon);

	DXCommon* GetDxCommon()const { return dxCommon_; }

	void DrawSpriteCommon();

private:
	void CreateRootSignature();

	void CreateGraphicsPipelineState();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	DXCommon* dxCommon_;
};

