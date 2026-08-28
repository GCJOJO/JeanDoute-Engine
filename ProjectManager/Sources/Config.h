#pragma once
#include <Engine/Core/Core.h>
#include <vector>
#include <filesystem>

namespace Engine
{
	class ProjectManagerConfig
	{
	public:
		void Save();
		static Ref<ProjectManagerConfig> Load();

		static Ref<ProjectManagerConfig> GetConfig() { return m_Config; };

	public:
		std::filesystem::path m_EditorPath;
		std::vector<std::filesystem::path> m_RecentProjects;

		inline static Ref<ProjectManagerConfig> m_Config;
	};
}