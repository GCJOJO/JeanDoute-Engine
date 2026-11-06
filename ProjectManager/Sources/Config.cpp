#include "Config.h"
#include "Engine/Utils/PlatformUtils.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Engine
{
	const std::filesystem::path configPath = Filesystem::GetUserDirectory() / "Config" / "ProjectManager.config";; // %APPDATA%/Local/JeanDoute-Engine/Config/ProjectManager.yaml
	
	void ProjectManagerConfig::Save()
	{
		std::filesystem::path configDir = configPath.parent_path();

		if (!std::filesystem::exists(configDir)) std::filesystem::create_directories(configDir);

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "ProjectManager";
			{
				out << YAML::BeginMap; // ProjectManager

				out << YAML::Key << "EditorPath" << YAML::Value << m_EditorPath.string().c_str();

				out << YAML::Key << "RecentProjects";
				{
					out << YAML::BeginSeq;
					
					for (const auto& project : m_RecentProjects)
						out << YAML::Value << project.string().c_str();
					
					out << YAML::EndSeq;
				}

				out << YAML::EndMap; // ProjectManager
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(configPath);
		fout << out.c_str();
	}


	SharedPtr<ProjectManagerConfig> ProjectManagerConfig::Load()
	{
		SharedPtr<ProjectManagerConfig> config = CreateSharedPtr<ProjectManagerConfig>();

		if (!std::filesystem::exists(configPath))
		{
			ENGINE_CORE_WARN("Couldn't find {}", configPath.string());
			return config;
		}

		YAML::Node data;

		try
		{
			data = YAML::LoadFile(configPath.string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load project file {}. \n{}", configPath.string(), e.what());
			return config;
		}

		auto configNode = data["ProjectManager"];
		if (!configNode) return config;

		config->m_EditorPath = configNode["EditorPath"].as<std::string>();

		for (const auto& project : configNode["RecentProjects"])
		{
			config->m_RecentProjects.push_back(project.as<std::string>());
		}
		return config;
	}
}