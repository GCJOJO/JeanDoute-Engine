#pragma once

#include <unordered_set>

namespace Engine
{
	template<typename T>
	class NodeRegistry
	{
	public:
		static NodeRegistry& Get()
		{
			static NodeRegistry instance;
			return instance;
		}

		void Register(T* node) 
		{ 
			auto [it, inserted] = m_Lookup.insert(node);
			if(inserted)
				m_Nodes.push_back(node); 
		}
		void Unregister(T* node) 
		{ 
			if(m_Lookup.erase(node) > 0)
				std::erase(m_Nodes, node); 
		}

		const std::vector<T*> GetNodes() const { return m_Nodes; }
		static std::vector<T*> GetAll() { return Get().GetNodes(); }

	private:
		std::vector<T*> m_Nodes;
		std::unordered_set<T*> m_Lookup;
	};

	// Merci ClaudeSlop
	template<typename Derived, typename... RegisterAs>
	class AutoRegister
	{
	public:
		AutoRegister()
		{
			(NodeRegistry<RegisterAs>::Get().Register(
				static_cast<RegisterAs*>(static_cast<Derived*>(this))), ...);
		}

		~AutoRegister()
		{
			(NodeRegistry<RegisterAs>::Get().Unregister(
				static_cast<RegisterAs*>(static_cast<Derived*>(this))), ...);
		}
	};
}