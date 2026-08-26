#include "enginepch.h"
#include "Engine/Assets/StaticMeshAsset.h"

#include "bgfx/bgfx.h"

namespace Engine
{
	Asset::Type StaticMeshAsset::GetType() const
	{
		return Asset::Type::StaticMesh;
	}
}