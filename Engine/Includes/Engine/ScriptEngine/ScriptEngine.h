#pragma once

#include <filesystem>
#include <map>
#include "Engine/Core/Assert.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/Node.h"

#include <Coral/Assembly.hpp>

#include <typeinfo>

namespace Engine
{
	enum class ScriptFieldType
	{
		None = 0,
		Float, Double,
		Bool, Char, Byte, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		String,
		Vector2, Vector3, Vector4, Color,
		Node,
	};

	/*struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		Coral::FieldInfo CoralField;
	};

	// Script Field + Data Storage
	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}

		template<typename T>
		T GetValue()
		{
			static_assert(sizeof(T) <= 16, "Type too large !");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large !");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class ScriptInstance;
		friend class ScriptEngine;
	};*/


	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;

		Coral::FieldInfo CoralField;
	};

	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Buffer, 0, sizeof(m_Buffer));
		}
	public:
		template<typename T>
		const T GetValue() const
		{
			static_assert(sizeof(T) <= 16, "Type too large !");
			return *(T*)m_Buffer;
		}

		template<typename T>
		void SetValue(T value)
		{
			static_assert(sizeof(T) <= 16, "Type too large !");
			memcpy(m_Buffer, &value, sizeof(T));
		}
	private:
		uint8_t m_Buffer[16];

		friend class ScriptInstance;
		friend class ScriptEngine;
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;


	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& classFullName, bool isCore = false);

		template<typename... TArgs>
		Coral::ManagedObject Instantiate(TArgs ... InArguments);
		const bool GetMethod(const std::string& name, int parameterCount, std::vector<Coral::MethodInfo>& foundMethod);

		const bool HasMethod(const std::string& name, const std::vector<ScriptFieldType>& parameterTypes);

		const std::map<std::string, ScriptField>& GetFields() { return m_Fields; }
	private:
		std::string m_ClassFullName;

		std::map<std::string, ScriptField> m_Fields;

		Coral::Type m_CoralType;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Ref<ScriptClass> scriptClass, Node* node);

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);

		Ref<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			//static_assert(sizeof(T) <= 16, "Type too large !");

			/*bool success = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!success)
				return T();

			return *(T*)s_FieldValueBuffer;*/
			return m_Instance.GetFieldValue<T>(name);
		}

		template<typename T>
		void SetFieldValue(const std::string& name, T value)
		{
			//static_assert(sizeof(T) <= 16, "Type too large !");
			m_Instance.SetFieldValue<T>(name, value);
			//SetFieldValueInternal(name, &value); 
		}

		Coral::ManagedObject& GetManagedObject() { return m_Instance; }

	private:
		bool GetFieldValueInternal(const std::string& name, void* buffer);
		bool SetFieldValueInternal(const std::string& name, const void* value);
	private:
		Ref<ScriptClass> m_ScriptClass;

		Coral::ManagedObject m_Instance;

		inline static char s_FieldValueBuffer[16];

		friend struct ScriptFieldInstance;
		friend class ScriptEngine;
	};

	class ScriptEngine
	{
	public:

		static void Init(const std::string& engineAssemlyPath = "Resources/Scripts/GamingEngine-ScriptCore.dll");
		static void Shutdown();

		//static bool LoadAssembly(const std::filesystem::path& filepath);
		static Coral::ManagedAssembly& LoadAssembly(const std::filesystem::path& filepath);
		static void ReloadAssembly();

		static Coral::ManagedAssembly* GetEngineAssembly();

		static bool NodeClassExists(const std::string& fullClassName);

		static Ref<ScriptClass> GetNodeClass(const std::string& name);
		static std::unordered_map<std::string, Ref<ScriptClass>> GetNodeClasses();
		static bool IsValidClass(const std::string& className);
		static ScriptFieldMap& GetScriptFieldMap(Node* node);

		static void OnRuntimeStart(Scene* scene);
		static void OnRuntimeStop();
		static void OnCreateNode(Node* node);
		static void OnUpdateNode(Node* node, Timestep ts);
		static void RequestStop();

		static void SetStopFunction(std::function<void()> newFunction);
		static void SetTimersActive(bool active);

		static Ref<ScriptInstance> GetNodeScriptInstance(UUID entityID);

		static Scene* GetSceneContext();

		static Coral::ManagedObject& GetManagedInstance(UUID id);

		static Coral::String CreateString(const std::string& string);
	private:
		static void InitCoral(const std::filesystem::path& coralDir);

		static void LoadAssemblyClasses();
		template<typename... TArgs>
		static Coral::ManagedObject InstantiateClass(Coral::Type& coralType, TArgs ... InArguments);

		friend class ScriptClass;
		friend class ScriptGlue;
	};

	namespace Utils
	{
		inline const char* ScriptFieldTypeToString(ScriptFieldType type)
		{
			switch (type)
			{
			case ScriptFieldType::None:		return "None";
			case ScriptFieldType::Float:	return "System.Single";
			case ScriptFieldType::Double:	return "System.Double";

			case ScriptFieldType::Bool:		return "System.Boolean";
			case ScriptFieldType::Char:		return "System.Char";
			case ScriptFieldType::Byte:		return "System.SByte";
			case ScriptFieldType::Short:	return "System.Int16";
			case ScriptFieldType::Int:		return "System.Int32";
			case ScriptFieldType::Long:		return "System.Int64";

			case ScriptFieldType::UByte:	return "System.Byte";
			case ScriptFieldType::UShort:	return "System.UInt16";
			case ScriptFieldType::UInt:		return "System.UInt32";
			case ScriptFieldType::ULong:	return "System.UInt64";

			case ScriptFieldType::String:	return "System.String";

			case ScriptFieldType::Vector2:	return "Engine.Vector2";
			case ScriptFieldType::Vector3:	return "Engine.Vector3";
			case ScriptFieldType::Vector4:	return "Engine.Vector4";

			case ScriptFieldType::Color:	return "Engine.Color";

			case ScriptFieldType::Node:		return "Engine.Node";
			}
			ENGINE_CORE_ASSERT(false, "Invalid field Type");
			return "None";
		}

		inline ScriptFieldType ScriptFieldTypeFromString(std::string_view fieldType)
		{
			if (fieldType == "None")			return ScriptFieldType::None;

			if (fieldType == "System.Single")	return ScriptFieldType::Float;
			if (fieldType == "System.Double")	return ScriptFieldType::Double;

			if (fieldType == "System.Boolean")	return ScriptFieldType::Bool;
			if (fieldType == "System.Char")		return ScriptFieldType::Char;
			if (fieldType == "System.SByte")	return ScriptFieldType::Byte;
			if (fieldType == "System.Int16")	return ScriptFieldType::Short;
			if (fieldType == "System.Int32")	return ScriptFieldType::Int;
			if (fieldType == "System.Int64")	return ScriptFieldType::Long;

			if (fieldType == "System.Byte")		return ScriptFieldType::UByte;
			if (fieldType == "System.UInt16")	return ScriptFieldType::UShort;
			if (fieldType == "System.UInt32")	return ScriptFieldType::UInt;
			if (fieldType == "System.UInt64")	return ScriptFieldType::ULong;

			if (fieldType == "System.String")	return ScriptFieldType::String;

			if (fieldType == "Engine.Vector2")	return ScriptFieldType::Vector2;
			if (fieldType == "Engine.Vector3")	return ScriptFieldType::Vector4;
			if (fieldType == "Engine.Vector4")	return ScriptFieldType::Vector4;

			if (fieldType == "Engine.Color")	return ScriptFieldType::Color;

			if (fieldType == "Engine.Node")		return ScriptFieldType::Node;

			ENGINE_CORE_ASSERT(false, "Invalid field Type");
			return ScriptFieldType::None;
		}
	}

}