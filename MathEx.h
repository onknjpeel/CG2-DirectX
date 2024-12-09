#pragma once
#define _USE_MATH_DEFINES

#include <cmath>
#include <math.h>

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

struct Matrix2x2 {
	float m[2][2];
};

struct Matrix3x3 {
	float m[3][3];
};

struct Matrix4x4 {
	float m[4][4];
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

#pragma region コタンジェント
float cot(float x);
#pragma endregion

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 MakeIdentity4x4();

Matrix4x4 MakeScaleMatrix(const Vector3& scale);


Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeRotateXMatrix(const float& rotate);
Matrix4x4 MakeRotateYMatrix(const float& rotate);
Matrix4x4 MakeRotateZMatrix(const float& rotate);

Matrix4x4 MakeRotateMatrix(const Vector3& rotate);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 Inverse(const Matrix4x4& m);

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

#pragma region 平行投影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
#pragma endregion