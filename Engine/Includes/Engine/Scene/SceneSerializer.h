#pragma once
#include "Scene.h"

namespace Engine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const SharedPtr<Scene>& scene) : m_Scene(scene) {};

		void Serialize(const std::filesystem::path& filepath);
		void SerializeRuntime(const std::filesystem::path& filepath);

		bool Deserialize(const std::filesystem::path& filepath);
		bool DeserializeRuntime(const std::filesystem::path& filepath);
	private:
		SharedPtr<Scene> m_Scene;
	};
}