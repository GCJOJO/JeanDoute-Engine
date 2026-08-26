#pragma once
#include "Engine/Assets/Asset.h"

namespace Engine
{
	class StaticMeshAsset : public Asset
	{
	public:
		struct Vertex {
			float x, y, z;
		};

		struct Normal {
			float nx, ny, nz;
		};

		struct UV {
			float u, v;
		};

		struct MeshData
		{
			std::vector<Vertex> vertices{ };
			std::vector<Normal> normals{ };
			std::vector<UV> uvs{ };
			std::vector<uint16_t> indices{ };
		};


		Type GetType() const override;

		StaticMeshAsset::MeshData GetMesh(uint32_t meshId) const 
		{
			if (meshId < m_Meshes.size())
				return m_Meshes[meshId];
			return { };
		};

		const std::vector<StaticMeshAsset::MeshData>& GetMeshes() const { return m_Meshes; };
	private:
		std::vector<MeshData> m_Meshes{ };
		friend class MeshImporter;
	};
}