#pragma once

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

		void Register(T* node) { m_Nodes.push_back(node); }
		void Unregister(T* node) { std::erase(m_Nodes, node); }

		const std::vector<T*> GetNodes() const { return m_Nodes; }
		static std::vector<T*> GetAll() { return Get().GetNodes(); }

	private:
		std::vector<T*> m_Nodes;
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