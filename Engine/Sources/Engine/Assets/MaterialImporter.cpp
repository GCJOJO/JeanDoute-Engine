#include "enginepch.h"
#include "Engine/Assets/MaterialImporter.h"
#include "Engine/Project/Project.h"
#include "Engine/Rendering/ShaderManager.h"

namespace Engine
{

	Ref<Material> MaterialImporter::LoadMaterial(AssetHandle handle, const AssetMetadata& metadata)
	{
		return ImportMaterial(Project::GetActiveAssetDirectory() / metadata.AssetPath);
	}

	Ref<Material> MaterialImporter::ImportMaterial(const std::filesystem::path& path)
	{
		const std::filesystem::path directory = path.parent_path();
		const std::filesystem::path vertexFile = directory / "vert.sc";
		const std::filesystem::path fragmentFile = directory / "frag.sc";

		bgfx::ProgramHandle program = ShaderManager::LoadProgram(vertexFile, fragmentFile);

		MaterialTemplate materialTemplate = MaterialTemplate::ParseMaterialTemplate(fragmentFile);
		return CreateRef<Material>(std::move(materialTemplate), program);
	}

}