#pragma once
#include "Asset.h"
#include "Engine/Core/Core.h"

#include <filesystem>

namespace Engine
{
	struct AssetMetadata
	{
		Asset::Type Type = Asset::Type::None;
		std::filesystem::path AssetPath;

		operator bool() const { return Type != Asset::Type::None; }
		Ref<void> SpecificFileData{ nullptr };
	public:
		template <typename T>
		T* GetSpecificMetaData()
		{
			return static_cast<T*>(SpecificFileData);
		}
	};

	struct TextureMetadata
	{
		int32_t Width, Height, Channels;
	};
	
	struct StaticMeshMetadata {};

	struct SceneMetadata {};
}