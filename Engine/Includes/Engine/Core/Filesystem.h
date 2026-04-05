#pragma once
#include "Engine/Core/Buffer.h"

namespace Engine
{
	// TODO : Platforms
	class Filesystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};
}