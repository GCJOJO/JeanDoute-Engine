#pragma once
#include "RefCounted.h"

namespace Engine
{
	class Resource : RefCounted
	{
	public:

		virtual void Reload();
		void Duplicate()
		{
			
		}

	private:
		std::filesystem::path m_ResourcePath;
		UUID m_ResourceId;
		StringName m_ResourceName;

		// Signal m_ChangedSignal;
	};
}