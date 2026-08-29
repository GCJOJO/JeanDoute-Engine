#include "enginepch.h"
#include "Engine/Types/StringName.h"

#include <xutility>

namespace Engine
{
	struct Table
	{
		std::unordered_map<std::string, uint32_t> forward;
		std::vector<std::string> reverse;

		static Table& Get()
		{
			static Table table;
			return table;
		}
	};

	const std::string& StringName::ToString() const
	{
		return Table::Get().reverse[m_Id];
	}

	uint32_t StringName::Intern(const std::string& str)
	{
		Table& table = Table::Get();

		auto it = table.forward.find(str);
		if (it != table.forward.end())
			return it->second;
		

		uint32_t newId = static_cast<uint32_t>(table.reverse.size());
		table.reverse.push_back(str);
		table.forward[str] = newId;
		return newId;
	}
}