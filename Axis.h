#pragma once
#include "IModel.h"

class Axis :public IModel {
public:
	void Init(DXCommon* pointer) {
		dxCommon = pointer;
	}
	void LoadModel()override;
	void CreateModel()override;
	void SetModel()override;
	void DrawModel()override;

	void SetTransformMatData(TransformationMatrix* matData) {
		transformationMatrixData = matData;
	}
};