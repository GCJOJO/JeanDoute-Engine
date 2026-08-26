#pragma once
#include "Engine/Core/UUID.h"
#include <cstdint>
#include <string_view>

namespace Engine
{
	using AssetHandle = UUID;

	class Asset
	{
	public:

		enum class Type : uint32_t
		{
			None = 0,
			Scene,
			Texture2D,
			Shader,
			StaticMesh,
		};

		const AssetHandle& GetAssetHandle() const { return m_AssetHandle; }

		static std::string_view AssetTypeToString(Asset::Type& assetType);
		static Asset::Type StringToAssetType(std::string_view& assetType);

		virtual Type GetType() const = 0;
	private:
		AssetHandle m_AssetHandle;
	};
}