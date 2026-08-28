#include "EditorConfig.h"
#include "Engine/Core/Log.h"
#include "Engine/Utils/PlatformUtils.h"

#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include "yaml-cpp/exceptions.h"

namespace Engine
{
	std::filesystem::path EditorConfig::LastProjectPath = "";
	const std::filesystem::path CONFIG_PATH = Filesystem::GetUserDirectory() / "Config" / "Editor.config";

	void EditorConfig::Serialize()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "LastProjectPath" << YAML::Value << LastProjectPath.generic_string().c_str();
		out << YAML::EndMap;

		std::ofstream fout(CONFIG_PATH);
		fout << out.c_str();
		fout.close();
	}

	bool EditorConfig::Deserialize()
	{
		if (!std::filesystem::exists(CONFIG_PATH))
		{
			ENGINE_CORE_WARN("Couldn't find editor.config file");
			return false;
		}
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(CONFIG_PATH.string());
			if (data["LastProjectPath"].IsDefined())
				LastProjectPath = data["LastProjectPath"].as<std::string>();
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load editor.config file. \n{}", e.what());
			return false;
		}
	}

}