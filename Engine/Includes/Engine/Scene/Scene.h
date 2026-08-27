#pragma once
#include "SceneTree.h"
#include "Engine/Assets/Asset.h"
#include "Engine/Core/Timestep.h"

namespace Engine
{
	class Scene : public Asset
	{
	public:
		Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		void UpdateScene(Timestep ts, int viewId);

		SceneTree& GetSceneTree() { return m_SceneTree; }

		Asset::Type GetType() const override { return Asset::Type::Scene; }

	private:
		SceneTree m_SceneTree;
	};
}