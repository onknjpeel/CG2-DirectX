#include "IModel.h"
#include <fstream>
#include <sstream>
#include <cassert>

MaterialData IModel::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
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

ModelData IModel::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
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