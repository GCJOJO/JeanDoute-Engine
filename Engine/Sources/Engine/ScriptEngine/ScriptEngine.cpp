#include "enginepch.h"

#include "Engine/ScriptEngine/ScriptEngine.h"
#include "Engine/Core/Application.h"
#include "Engine/Core/Buffer.h"
#include "Engine/Utils/PlatformUtils.h"
#include "Engine/ScriptEngine/ScriptGlue.h"
//#include "Engine/Project/Project.h"

#include <Coral/HostInstance.hpp>
#include <Coral/GC.hpp>
#include <Coral/Array.hpp>
#include <Coral/Attribute.hpp>
#include <Coral/MethodInfo.hpp>


#include "FileWatch.h"

#include <functional>

namespace Engine
{
#define COPY_FIELD_TYPE(nodeInstance, fieldName, fieldInstance, scriptFieldType, fieldType) \
	case ScriptFieldType::scriptFieldType: \
		nodeInstance->SetFieldValue<fieldType>(fieldName, fieldInstance.GetValue<fieldType>());

	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{"System.Single",	ScriptFieldType::Float },
		{"System.Double",	ScriptFieldType::Double },

		{"System.Boolean",	ScriptFieldType::Bool },
		{"System.Byte",		ScriptFieldType::Byte },
		{"System.Char",		ScriptFieldType::Char },
		{"System.Int16",	ScriptFieldType::Short },
		{"System.Int",		ScriptFieldType::Int },
		{"System.Int64",	ScriptFieldType::Long },

		{"System.UByte",	ScriptFieldType::UByte },
		{"System.UInt16",	ScriptFieldType::UShort },
		{"System.UInt",		ScriptFieldType::UInt },
		{"System.UInt64",	ScriptFieldType::ULong },

		{"System.String",	ScriptFieldType::String },

		{"Engine.Vector2",	ScriptFieldType::Vector2 },
		{"Engine.Vector3",	ScriptFieldType::Vector3 },
		{"Engine.Vector4",	ScriptFieldType::Vector4 },
		{"Engine.Color",	ScriptFieldType::Color },

		{"Engine.Node",		ScriptFieldType::Node },
	};


	struct ScriptEngineData
	{
		Coral::HostInstance hostInstance;
		Coral::AssemblyLoadContext loadContext;
		Coral::HostSettings hostSettings;

		std::string EngineAssemblyPath;
		Coral::ManagedAssembly* EngineAssembly = nullptr;
		Coral::ManagedAssembly* AppAssembly = nullptr;

		std::filesystem::path CoreAssemblyFilepath;
		std::filesystem::path AppAssemblyFilepath;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Ref<ScriptClass>> NodeClasses;
		std::unordered_map<UUID, Ref<ScriptInstance>> EntityInstances;

		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		Scope<filewatch::FileWatch<std::string>> AppAssemblyFileWatcher;
		bool AssemblyReloadPending = false;

#ifdef ENGINE_DEBUG
		bool EnableDebugging = true;
#else
		bool EnableDebugging = false;
#endif

		// Runtime
		Scene* SceneContext = nullptr;
		std::function<void()> OnStopFunction;
	};

	static ScriptEngineData* s_ScriptEngineData = nullptr;

	static void MessageCallback(std::string_view InMessage, Coral::MessageLevel InLevel)
	{
		switch (InLevel)
		{
		case Coral::MessageLevel::Trace:
			ENGINE_CORE_TRACE("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::Info:
			ENGINE_CORE_INFO("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::Warning:
			ENGINE_CORE_WARN("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::Error:
			ENGINE_CORE_ERROR("[Script] : {}", InMessage);
			break;
		case Coral::MessageLevel::All:
			ENGINE_CORE_INFO("[Script] : {}", InMessage);
			break;
		default:
			ENGINE_CORE_INFO("[Script] : {}", InMessage);
			break;
		}
	}

	static void ExceptionCallback(std::string_view InMessage)
	{
		ENGINE_CORE_ERROR("Unhandled native exception : {}", InMessage);
	}

	static void OnAppAssemblyFileSystemEvent(const std::string& path, const filewatch::Event change_type)
	{
		if (!s_ScriptEngineData->AssemblyReloadPending && change_type == filewatch::Event::modified)
		{
			s_ScriptEngineData->AssemblyReloadPending = true;

			Application::Get().SubmitToMainThread([]()
				{
					s_ScriptEngineData->AppAssemblyFileWatcher.reset();
					ScriptEngine::ReloadAssembly();
				});
		}
	}

	void ScriptEngine::ReloadAssembly()
	{
		auto engineAssemblyPath = s_ScriptEngineData->EngineAssemblyPath;
		Shutdown();
		Init(engineAssemblyPath);
	}

	Coral::ManagedAssembly* ScriptEngine::GetEngineAssembly()
	{
		return s_ScriptEngineData->EngineAssembly;
	}

	void ScriptEngine::Init(const std::string& engineAssemblyPath /* = std::string()*/)
	{
		s_ScriptEngineData = new ScriptEngineData();
		s_ScriptEngineData->EngineAssemblyPath = engineAssemblyPath;

		std::filesystem::path engineAssemblyDir = std::filesystem::path(engineAssemblyPath).parent_path();

		InitCoral(engineAssemblyDir);

		s_ScriptEngineData->EngineAssembly = &LoadAssembly(engineAssemblyPath);
		if (s_ScriptEngineData->EngineAssembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			ENGINE_CORE_ERROR("[Scripting Engine] Couldn't load Coral Assembly");
			return;
		}

		s_ScriptEngineData->EntityClass = ScriptClass("Engine.Entity", true);
		ScriptGlue::RegisterFunctions(*s_ScriptEngineData->EngineAssembly);
		ScriptGlue::RegisterNodes();

		/*/auto scriptModulePath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetConfig().ScriptModulePath;
		s_ScriptEngineData->AppAssembly = &LoadAssembly(scriptModulePath);
		if (s_ScriptEngineData->AppAssembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			ENGINE_CORE_ERROR("[Scripting Engine] Couldn't load App Assembly");
			return;
		}*/

		//LoadAssemblyClasses();
	}

	Coral::ManagedAssembly& ScriptEngine::LoadAssembly(const std::filesystem::path& assemblyPath)
	{
		auto& assembly = s_ScriptEngineData->loadContext.LoadAssembly(assemblyPath.string());

		s_ScriptEngineData->AppAssemblyFileWatcher = CreateScope<filewatch::FileWatch<std::string>>(
			assemblyPath.string(), OnAppAssemblyFileSystemEvent);
		s_ScriptEngineData->AssemblyReloadPending = false;

		return assembly;
	}

	void ScriptEngine::Shutdown()
	{
		if (s_ScriptEngineData == nullptr)
		{
			return;
		}

		//ShutdownMono();
		s_ScriptEngineData->hostInstance.UnloadAssemblyLoadContext(s_ScriptEngineData->loadContext);
		Coral::GC::Collect();
		s_ScriptEngineData->hostInstance.Shutdown();
		delete s_ScriptEngineData;
	}

	void ScriptEngine::InitCoral(const std::filesystem::path& coralDir)
	{
		//Application& app = Application::Get();
		//auto exePath = std::filesystem::path(app.GetExecutablePath());
		//auto coralDir = std::filesystem::path(exePath.parent_path() / "Resources" / "Scripts").string();

		s_ScriptEngineData->hostSettings.CoralDirectory = coralDir.string();
		s_ScriptEngineData->hostSettings.MessageCallback = MessageCallback;
		s_ScriptEngineData->hostSettings.ExceptionCallback = ExceptionCallback;

		auto initStatus = s_ScriptEngineData->hostInstance.Initialize(s_ScriptEngineData->hostSettings);
		if (initStatus != Coral::CoralInitStatus::Success)
		{
			ENGINE_ASSERT(false, "Coral not initalized");
			return;
		}

		s_ScriptEngineData->loadContext = s_ScriptEngineData->hostInstance.CreateAssemblyLoadContext("RuntimeContext");
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_ScriptEngineData->NodeClasses.clear();

		auto nodeType = s_ScriptEngineData->EngineAssembly->GetType("Engine.Node");

		/*for (const auto& method : entityType.GetMethods())
			ENGINE_CORE_INFO("App Type {} has method {}", std::string(entityType.GetFullName()), std::string(method.GetName()));*/

		const auto& appTypes = s_ScriptEngineData->AppAssembly->GetTypes();

		for (const auto& appType : appTypes)
		{
			if (appType->IsSubclassOf(nodeType))
			{
				std::string typeName = std::string(appType->GetFullName());
				//ENGINE_CORE_INFO("App Type {} is subclass of Engine.Entity", typeName);
				s_ScriptEngineData->NodeClasses[appType->GetFullName()] = CreateRef<ScriptClass>(typeName);

				/*for (const auto& method : appType->GetMethods())
					ENGINE_CORE_INFO("App Type {} has method {}", typeName, std::string(method.GetName()));*/
			}
		}

	}

	bool ScriptEngine::EntityClassExists(const std::string& fullClassName)
	{
		return s_ScriptEngineData->NodeClasses.find(fullClassName) != s_ScriptEngineData->NodeClasses.end();
	}

	Ref<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (s_ScriptEngineData->NodeClasses.find(name) == s_ScriptEngineData->NodeClasses.end())
			return nullptr;

		return s_ScriptEngineData->NodeClasses.at(name);
	}

	std::unordered_map<std::string, Ref<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_ScriptEngineData->NodeClasses;
	}

	bool ScriptEngine::IsValidClass(const std::string& className)
	{
		return GetEntityClasses().find(className) != GetEntityClasses().end();
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Node* node)
	{
		ENGINE_CORE_ASSERT(node);
		UUID entityId = node->GetId();
		return s_ScriptEngineData->EntityScriptFields[entityId];
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
		s_ScriptEngineData->SceneContext = scene;
		SetTimersActive(true);
	}

	void ScriptEngine::OnRuntimeStop()
	{
		SetTimersActive(false);
		s_ScriptEngineData->SceneContext = nullptr;
		s_ScriptEngineData->EntityInstances.clear();
		Coral::GC::Collect();
	}

	void ScriptEngine::SetTimersActive(bool active)
	{
		auto utils = s_ScriptEngineData->EngineAssembly->GetLocalType("Engine.Utils");
		if (active)
			utils.InvokeStaticMethod("SetTimersActive");
		else
			utils.InvokeStaticMethod("SetTimersInactive");
	}

	void ScriptEngine::OnCreateNode(Node* node)
	{
		/*const auto& sc = entity.GetComponent<ScriptComponent>();
		if (ScriptEngine::EntityClassExists(sc.ClassName))
		{
			UUID entityId = entity.GetUUID();

			Ref<ScriptInstance> instance = CreateRef<ScriptInstance>(s_ScriptEngineData->EntityClasses[sc.ClassName], entity);
			s_ScriptEngineData->EntityInstances[entityId] = instance;

			// Copy Field values
			if (s_ScriptEngineData->EntityScriptFields.find(entityId) != s_ScriptEngineData->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_ScriptEngineData->EntityScriptFields.at(entityId);
				for (const auto& [name, fieldInstance] : fieldMap)
				{
					switch (fieldInstance.Field.Type)
					{
						COPY_FIELD_TYPE(instance, name, fieldInstance, Float, float);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Double, double);

						COPY_FIELD_TYPE(instance, name, fieldInstance, Bool, bool);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Char, char);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Byte, int8_t);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Short, int16_t);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Int, int32_t);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Long, int64_t);

						COPY_FIELD_TYPE(instance, name, fieldInstance, UByte, uint8_t);
						COPY_FIELD_TYPE(instance, name, fieldInstance, UShort, uint16_t);
						COPY_FIELD_TYPE(instance, name, fieldInstance, UInt, uint32_t);
						COPY_FIELD_TYPE(instance, name, fieldInstance, ULong, uint64_t);

						COPY_FIELD_TYPE(instance, name, fieldInstance, String, uint64_t);

						COPY_FIELD_TYPE(instance, name, fieldInstance, Vector2, glm::vec2);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Vector3, glm::vec3);
						COPY_FIELD_TYPE(instance, name, fieldInstance, Vector4, glm::vec4);

						COPY_FIELD_TYPE(instance, name, fieldInstance, Entity, UUID);
					}
				}
			}
			instance->InvokeOnCreate();
		}
		}*/
	}

	void ScriptEngine::OnUpdateNode(Node* node, Timestep ts)
	{
		UUID entityUUID = node->GetId();
		if (s_ScriptEngineData->EntityInstances.find(entityUUID) != s_ScriptEngineData->EntityInstances.end())
		{
			Ref<ScriptInstance> instance = s_ScriptEngineData->EntityInstances[node->GetId()];
			instance->InvokeOnUpdate((float)ts);
		}
		else
			ENGINE_CORE_ERROR("Couldn't find ScriptInstance for entity {}", node->GetName());
	}

	Ref<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityID)
	{
		auto it = s_ScriptEngineData->EntityInstances.find(entityID);
		if (it == s_ScriptEngineData->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	Scene* ScriptEngine::GetSceneContext()
	{
		return s_ScriptEngineData->SceneContext;
	}

	template<typename... TArgs>
	Coral::ManagedObject ScriptEngine::InstantiateClass(Coral::Type& coralType, TArgs ... InArguments)
	{
		return coralType.CreateInstance(std::forward<TArgs>(InArguments)...);
	}

	Coral::ManagedObject& ScriptEngine::GetManagedInstance(UUID id)
	{
		ENGINE_CORE_ASSERT(s_ScriptEngineData->EntityInstances.find(id) != s_ScriptEngineData->EntityInstances.end());
		return s_ScriptEngineData->EntityInstances.at(id)->GetManagedObject();
	}

	Coral::String ScriptEngine::CreateString(const std::string& string)
	{
		return Coral::String::New(string);
	}

	void ScriptEngine::RequestStop()
	{
		s_ScriptEngineData->OnStopFunction();
	}

	void ScriptEngine::SetStopFunction(std::function<void()> newFunction)
	{
		s_ScriptEngineData->OnStopFunction = newFunction;
	}

	ScriptClass::ScriptClass(const std::string& classFullName, bool isCore /* = false */)
		: m_ClassFullName(classFullName)
	{
		auto& classAssembly = isCore ? s_ScriptEngineData->EngineAssembly : s_ScriptEngineData->AppAssembly;
		m_CoralType = classAssembly->GetType(m_ClassFullName);

		m_Fields.clear();
		for (auto& field : m_CoralType.GetFields())
		{
			if (field.GetAccessibility() != Coral::TypeAccessibility::Public)
				continue;
			ENGINE_CORE_WARN("{} : {}", std::string(field.GetType().GetFullName()), std::string(field.GetName()));

			ScriptField scriptField;
			scriptField.Type = Utils::ScriptFieldTypeFromString(std::string(field.GetType().GetFullName()));
			scriptField.Name = field.GetName();
			scriptField.CoralField = field;

			m_Fields[field.GetName()] = scriptField;
		}
	}

	template<typename... TArgs>
	Coral::ManagedObject ScriptClass::Instantiate(TArgs ... InArguments)
	{
		return ScriptEngine::InstantiateClass(m_CoralType, std::forward<TArgs>(InArguments)...);
	}

	const bool ScriptClass::GetMethod(const std::string& name, int parameterCount, std::vector<Coral::MethodInfo>& foundMethods)
	{
		foundMethods = std::vector<Coral::MethodInfo>();
		foundMethods.clear();
		auto methods = m_CoralType.GetMethods();
		for (auto& method : methods)
			if (method.GetName() == name && method.GetParameterTypes().size() == parameterCount)
				foundMethods.push_back(method);
		return foundMethods.size() >= 1;
	}

	const bool ScriptClass::HasMethod(const std::string& name, const std::vector<ScriptFieldType>& parameterTypes)
	{
		std::vector<Coral::MethodInfo> foundMethods;
		if (GetMethod(name, parameterTypes.size(), foundMethods))
		{
			for (auto& foundMethod : foundMethods)
			{
				bool identicalParameterTypes = true;
				const auto& methodParameterTypes = foundMethod.GetParameterTypes();
				for (int i = 0; i < parameterTypes.size(); i++)
				{
					if (methodParameterTypes[i]->GetFullName() != Utils::ScriptFieldTypeToString(parameterTypes[i]))
					{
						identicalParameterTypes = false;
						break;
					}
				}

				if (identicalParameterTypes)
					return true;
			}
		}
		return false;
	}

	ScriptInstance::ScriptInstance(Ref<ScriptClass> scriptClass, Node* node)
		: m_ScriptClass(scriptClass)
	{
		UUID entityId = node->GetId();
		m_Instance = scriptClass->Instantiate((uint64_t)entityId);
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_ScriptClass->HasMethod("OnCreate", {}))
			m_Instance.InvokeMethod("OnCreate");
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		if (m_ScriptClass->HasMethod("OnUpdate", { ScriptFieldType::Float }))
			m_Instance.InvokeMethod("OnUpdate", ts);
	}

	bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
	{
		const auto fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		//const ScriptField& field = it->second;
		//mono_field_get_value(m_Instance, field.MonoField, buffer);
		buffer = m_Instance.GetFieldValue<void*>(name);
		return true;
	}

	bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
	{
		const auto fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);
		if (it == fields.end())
			return false;

		//const ScriptField& field = it->second;
		//mono_field_set_value(m_Instance, field.MonoField, (void*)value);
		m_Instance.SetFieldValue(name, value);
		return true;
	}
}