#include "enginepch.h"
#include "Engine/Assets/Asset.h"

namespace Engine
{
	std::string_view Asset::AssetTypeToString(Asset::Type& assetType)
	{
		switch (assetType)
		{
		case Engine::Asset::Type::None:			return "AssetType::None";
		case Engine::Asset::Type::Scene:		return "AssetType::Scene";
		case Engine::Asset::Type::Texture2D:	return "AssetType::Texture2D";
		//case Engine::Asset::Type::Audio:		return "AssetType::Audio";
		}

		return "AssetType::<invalid>";
	}

	Asset::Type Asset::StringToAssetType(std::string_view& assetType)
	{
		if (assetType == "AssetType::Scene")		return Asset::Type::Scene;
		if (assetType == "AssetType::Texture2D")	return Asset::Type::Texture2D;
		//if (assetType == "AssetType::Audio")		return Asset::Type::Audio;

		return Asset::Type::None;
	}

}