#pragma once
#include "Engine/Types/StringName.h"

#include <functional>

// Merci claudeslop
namespace Engine
{
	class Node;

	class NodeFactory
	{
	public:
		using CreateNodeFn = std::function<Node* ()>;

		static NodeFactory& Get()
		{
			static NodeFactory factory;
			return factory;
		}

		void RegisterNodeClass(const StringName& className, CreateNodeFn function)
		{
			m_Factories[className] = function;
		}

		Node* CreateNode(const StringName& className)
		{
			auto it = m_Factories.find(className);
			if (it != m_Factories.end())
				return it->second();
			return nullptr;
		}

	private:
		std::unordered_map<StringName, CreateNodeFn> m_Factories;
	};

	template<typename T>
	struct NodeRegistrar
	{
		static bool registered;
	};

	template<typename T>
	bool NodeRegistrar<T>::registered = [] {
		NodeFactory::Get().RegisterNodeClass(T::get_static_class_name(), [] {
			return new T();
			});
		return true;
		}();
}