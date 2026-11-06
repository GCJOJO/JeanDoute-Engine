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

		static SharedPtr<Scene> Copy(SharedPtr<Scene> other);

		void StartScene();
		void UpdateScene(Timestep ts, int viewId);
		void StopScene();

		SceneTree& GetSceneTree() { return m_SceneTree; }

		Asset::Type GetType() const override { return Asset::Type::Scene; }

	private:
		SceneTree m_SceneTree;
	};
}