#include "enginepch.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/UUID.h"
#include "Engine/Scene/Node.h"
#include "Engine/Project/Project.h"
#include "Engine/ScriptEngine/ScriptEngine.h"

#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include "yaml-cpp/exceptions.h"

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
#ifdef ENGINE_DEBUG_VERBOSE
			const auto& mark = node.Mark();
			//ENGINE_CORE_INFO("{}:{}:{} is a Sequence ? {}. Node Size : {}", mark.line, mark.column, mark.pos, node.IsSequence(), node.size());
			for (int i = 0; i <= node.size() - 1; i++)
				ENGINE_CORE_INFO("{}", node[i].as<std::string>());
#endif

			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Engine::UUID>
	{
		static Node encode(const Engine::UUID& rhs)
		{
			Node node;
			node.push_back((uint64_t)rhs);
			return node;
		}

		static bool decode(const Node& node, Engine::UUID& rhs)
		{
			rhs = node.as<uint64_t>();
			return true;
		}
	};

}

namespace Engine
{
#define WRITE_FIELD_TYPE(FieldType, Type)					\
				case ScriptFieldType::FieldType:			\
					out << scriptField.GetValue<Type>();	\
					break

#define READ_FIELD_TYPE(FieldType, Type)				\
	case ScriptFieldType::FieldType:					\
	{													\
		Type data = scriptField["Data"].as<Type>();     \
		fieldInstance.SetValue(data);					\
		break;											\
	}													\

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	void SceneSerializer::Serialize(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		
		for (const Node* node : NodeRegistry<Node>::Get().GetNodes())
		{
			node->SaveNode(out);
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::filesystem::path& filepath)
	{
		// Not Implemented
		ENGINE_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
			ENGINE_CORE_WARN("Couldn't find {}", filepath.string());
			return false;
		}

		YAML::Node data;

		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			ENGINE_CORE_ERROR("Failed to load .scene file {}. \n{}", filepath.string(), e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ENGINE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		/*if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				//ENGINE_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_Scene->CreateEntity(uuid, name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					// Entities always have transforms
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto parentComponent = entity["ParentComponent"];
				if (parentComponent)
				{
					auto& pc = deserializedEntity.GetComponent<ParentComponent>();

					for (const auto& childId : parentComponent)
						pc.ChildrenIds.push_back(childId.as<uint64_t>());

				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();

					const auto& cameraProps = cameraComponent["Camera"];
					cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto scriptComponent = entity["ScriptComponent"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ClassName = scriptComponent["ClassName"].as<std::string>();

					auto scriptFields = scriptComponent["ScriptFields"];
					if (scriptFields)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(sc.ClassName);
						if (entityClass)
						{
							const auto& fields = entityClass->GetFields();
							auto& entityFields = ScriptEngine::GetScriptFieldMap(deserializedEntity);

							for (auto scriptField : scriptFields)
							{
								std::string name = scriptField["Name"].as<std::string>();
								std::string typeString = scriptField["Type"].as<std::string>();
								ScriptFieldType type = Utils::ScriptFieldTypeFromString(typeString);

								ScriptFieldInstance& fieldInstance = entityFields[name];

								ENGINE_CORE_ASSERT(fields.find(name) != fields.end());
								if (fields.find(name) == fields.end())
									continue;

								fieldInstance.Field = fields.at(name);
								try
								{
									switch (type)
									{
										READ_FIELD_TYPE(Float, float);
										READ_FIELD_TYPE(Double, double);

										READ_FIELD_TYPE(Bool, bool);
										READ_FIELD_TYPE(Char, char);
										READ_FIELD_TYPE(Byte, int8_t);
										READ_FIELD_TYPE(Short, int16_t);
										READ_FIELD_TYPE(Int, int32_t);
										READ_FIELD_TYPE(Long, int64_t);

										READ_FIELD_TYPE(UByte, uint8_t);
										READ_FIELD_TYPE(UShort, uint16_t);
										READ_FIELD_TYPE(UInt, uint32_t);
										READ_FIELD_TYPE(ULong, uint64_t);

										//FIELD_TYPE(String,std::string);

										//(Vector2, glm::vec2);
										//(Vector3, glm::vec3);
										//(Vector4, glm::vec4);

										READ_FIELD_TYPE(Vector2, glm::vec2);
										READ_FIELD_TYPE(Vector3, glm::vec3);
										//READ_FIELD_TYPE(Vector4, glm::vec4);
										//READ_FIELD_TYPE(Color,   glm::vec4);



										READ_FIELD_TYPE(Entity, UUID);
									}
								}
								catch (std::exception& ex)
								{
									ENGINE_CORE_WARN(ex.what());
								}
							}
						}
					}
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();

					// NOTE : Legacy, could try to find something that matches in the asset registry ?
					/ *if (spriteRendererComponent["TexturePath"])
					{
						std::string texturePath = spriteRendererComponent["TexturePath"].as<std::string>();
						auto path = Project::GetAssetFileSystemPath(texturePath);
						src.Texture = Texture2D::Create(path.string());
					}* /
					if (spriteRendererComponent["TextureHandle"])
						src.TextureHandle = spriteRendererComponent["TextureHandle"].as<AssetHandle>();

					if (spriteRendererComponent["TilingFactor"])
						src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<CircleRendererComponent>();
					src.Color = circleRendererComponent["Color"].as<glm::vec4>();
					src.Thickness = circleRendererComponent["Thickness"].as<float>();
					src.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto rigidBody2DComponent = entity["RigidBody2DComponent"];
				if (rigidBody2DComponent)
				{
					auto& rb2d = deserializedEntity.AddComponent<RigidBody2DComponent>();
					rb2d.Type = RigidBody2DBodyTypeFromString(rigidBody2DComponent["BodyType"].as<std::string>());
					rb2d.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
				}

				auto rigidBody3DComponent = entity["RigidBody3DComponent"];
				if (rigidBody3DComponent)
				{
					auto& rb3d = deserializedEntity.AddComponent<RigidBody3DComponent>();
					rb3d.Type = RigidBody3DBodyTypeFromString(rigidBody3DComponent["BodyType"].as<std::string>());
					rb3d.FixedRotation = rigidBody3DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();

					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2d.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();

					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto cubeCollider3DComponent = entity["CubeCollider3DComponent"];
				if (cubeCollider3DComponent)
				{
					auto& cc3d = deserializedEntity.AddComponent<CubeCollider3DComponent>();
					cc3d.Offset = cubeCollider3DComponent["Offset"].as<glm::vec3>();
					cc3d.Rotation = cubeCollider3DComponent["Size"].as<glm::vec3>();
					cc3d.Size = cubeCollider3DComponent["Size"].as<glm::vec3>();
					cc3d.Static = cubeCollider3DComponent["Static"].as<bool>();
					cc3d.Density = cubeCollider3DComponent["Density"].as<float>();
					cc3d.Friction = cubeCollider3DComponent["Friction"].as<float>();
					cc3d.Restitution = cubeCollider3DComponent["Restitution"].as<float>();
					cc3d.RestitutionThreshold = cubeCollider3DComponent["RestitutionThreshold"].as<float>();
				}

				auto textComponent = entity["TextComponent"];
				if (textComponent)
				{
					auto& tc = deserializedEntity.AddComponent<TextComponent>();
					tc.TextString = textComponent["Text"].as<std::string>();
					//tc.FontAsset = textComponent["Font"].as<std::string>();
					tc.Color = textComponent["Color"].as<glm::vec4>();
					tc.Kerning = textComponent["Kerning"].as<float>();
					tc.LineSpacing = textComponent["LineSpacing"].as<float>();
				}
			}
		}

		std::vector<Entity> sceneEntities = m_Scene->GetAllEntities();

		for (auto& entity : sceneEntities)
		{
			if (!entity) continue;

			for (auto& child : entity.GetChildren())
				child.GetComponent<ParentComponent>().ParentId = entity.GetUUID();
		}
*/

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::filesystem::path& filepath)
	{
		// Not Implemented
		ENGINE_CORE_ASSERT(false);
		return false;
	}

}