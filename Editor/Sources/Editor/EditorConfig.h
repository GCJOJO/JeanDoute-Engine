#pragma once
#include <filesystem>

namespace Engine
{
	class EditorConfig
	{
	public:
		static void Serialize();
		static bool Deserialize();

	public:
		static std::filesystem::path LastProjectPath;
	};
}