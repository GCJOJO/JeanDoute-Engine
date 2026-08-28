#pragma once
#include <spdlog/fmt/bundled/format.h>

namespace Engine
{
	class StringName
	{
	public:
		StringName() : m_Id(Intern("")) { }
		StringName(const char* str) : m_Id(Intern(std::string(str))), m_Length(strlen(str)) { }
		StringName(const std::string& str) : m_Id(Intern(str)), m_Length(str.length()) { }
		StringName(std::string_view str) : m_Id(Intern(std::string(str))), m_Length(str.length()) { }

		const std::string& ToString() const;
		explicit operator std::string() { return ToString(); }
		explicit operator const char*() const { return ToString().c_str(); }
		
		operator bool() const { return !IsEmpty(); }

		bool IsEmpty() const { return Length() == 0; }
		size_t Length() const { return m_Length; }

		bool operator==(const StringName& other) const { return m_Id == other.m_Id; }
		bool operator!=(const StringName& other) const { return m_Id != other.m_Id; }


		bool operator==(const std::string& other) const { return ToString() == other; }
		bool operator!=(const std::string& other) const { return !(*this == other); }

		bool operator==(const char* other) const { return ToString() == other; }
		bool operator!=(const char* other) const { return !(*this == other); }

		size_t hash() const { return static_cast<size_t>(m_Id); }
		uint32_t id() const { return m_Id; }

	private:
		static uint32_t Intern(const std::string& str);

		uint32_t m_Id;
		size_t m_Length = 0;
	};

	inline bool operator==(const std::string& lhs, const StringName& rhs) { return rhs == lhs; }
	inline bool operator!=(const std::string& lhs, const StringName& rhs) { return rhs != lhs; }

	inline std::ostream& operator<<(std::ostream& os, const StringName& name)
	{
		return os << name.ToString();
	}
}

namespace std
{
	template<>
	struct hash<Engine::StringName>
	{
		size_t operator()(const Engine::StringName& s) const noexcept { return s.hash(); }
	};

	//https://stackoverflow.com/questions/59909102/stdformat-of-user-defined-types
	template <>
	struct std::formatter<Engine::StringName> : std::formatter<std::string> {
		auto format(Engine::StringName s, format_context& ctx) const {
			return formatter<string>::format(s.ToString(), ctx);
		}
	};
}

template <> struct fmt::formatter<Engine::StringName> : fmt::ostream_formatter {};