#pragma once
#include "Engine/Core/Core.h"
#include "Project.h"

namespace Engine
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(SharedPtr<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		SharedPtr<Project> m_Project;
	};
}