#include "enginepch.h"
#include "Engine/Types/StringName.h"

#include <xutility>

namespace Engine
{
	struct Table 
	{
		std::unordered_map<std::string, uint32_t> forward;
		std::vector<std::string> reverse;
	};

	static Table table;

	const std::string& StringName::ToString() const
	{
		return table.reverse[m_Id];
	}

	uint32_t StringName::Intern(const std::string& str)
	{
		auto it = table.forward.find(str);
		if (it != table.forward.end())
			return it->second;

		uint32_t newId = static_cast<uint32_t>(table.reverse.size());
		table.reverse.push_back(str);
		table.forward.emplace(str, newId);
		return newId;
	}
}