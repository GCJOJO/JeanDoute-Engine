#include "enginepch.h"
#include "Engine/Assets/MeshImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Engine
{

	Ref<StaticMeshAsset> MeshImporter::LoadStaticMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return nullptr;
	}

	Ref<StaticMeshAsset> MeshImporter::ImportStaticMesh(const std::filesystem::path& assetPath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(assetPath.string(),
			aiProcess_Triangulate | 
			aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace |
			aiProcess_ConvertToLeftHanded );

		if (!scene || !scene->HasMeshes())
		{
			ENGINE_CORE_ERROR("Couldn't load mesh {}", assetPath.string());
			return nullptr;
		}

		Ref<StaticMeshAsset> mesh = CreateRef<StaticMeshAsset>();
		for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
		{
			StaticMeshAsset::MeshData meshData{};
			const auto& aiMeshData = scene->mMeshes[meshIndex];
			for (unsigned vertex = 0; vertex < aiMeshData->mNumVertices; vertex++)
			{
				StaticMeshAsset::Vertex vertexData{ };
				vertexData.x = aiMeshData->mVertices[vertex].x;
				vertexData.y = aiMeshData->mVertices[vertex].y;
				vertexData.z = aiMeshData->mVertices[vertex].z;
				meshData.vertices.push_back(vertexData);

				if (aiMeshData->HasNormals())
				{
					StaticMeshAsset::Normal normalData{ };
					normalData.nx = aiMeshData->mNormals[vertex].x;
					normalData.ny = aiMeshData->mNormals[vertex].y;
					normalData.nz = aiMeshData->mNormals[vertex].z;
					meshData.normals.push_back(normalData);
				}

				if (aiMeshData->HasTextureCoords(0))
				{
					StaticMeshAsset::UV uvData{ };
					uvData.u = aiMeshData->mTextureCoords[0][vertex].x;
					uvData.v = aiMeshData->mTextureCoords[0][vertex].y;
					meshData.uvs.push_back(uvData);
				}
			}

			meshData.indices.reserve(aiMeshData->mNumFaces * 3);

			for (unsigned face = 0; face < aiMeshData->mNumFaces; face++)
			{
				const aiFace& aiFaceData = aiMeshData->mFaces[face];
				for (unsigned index = 0; index < aiFaceData.mNumIndices; index++)
					meshData.indices.push_back(aiFaceData.mIndices[index]);
			}

			mesh->m_Meshes.push_back(meshData);
		}

		return mesh;
	}

}