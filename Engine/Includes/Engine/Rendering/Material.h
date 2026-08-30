#pragma once
#include "Engine/Types/StringName.h"
#include "Engine/Types/Vector.h"
#include "Engine/Core/Core.h"
#include "Engine/Assets/Asset.h"
#include "Engine/Rendering/Texture.h"

#include "bgfx/bgfx.h"
#include <unordered_map>
#include <filesystem>
#include <variant>

// Merci ClaudeSlop

namespace Engine
{
	struct MaterialParamDef
	{
		StringName UniformName;
		enum class ParamType : uint16_t { Float = 0, Vec4, Color, Texture } Type;
		int PackOffset = 0;
		std::optional<std::variant<Vec4, Ref<Texture>>> DefaultValue;
		std::optional<float> Min, Max;
		std::string Label = "";
	};

	struct MaterialTemplate
	{
		std::filesystem::path shaderPath;
		std::vector<MaterialParamDef> params;

		static MaterialTemplate ParseMaterialTemplate(const std::filesystem::path& sourcePath);
	};

	class Material : public Asset
	{
	public:
		Material(MaterialTemplate materialTemplate, bgfx::ProgramHandle program);

		bgfx::ProgramHandle GetProgram() const { return m_ProgramHandle; }
		bgfx::UniformHandle GetUniform(const StringName& name) const { return m_UniformHandles.at(name); }
		const MaterialTemplate& GetTemplate() const { return m_Template; }

		Asset::Type GetType() const override { return Asset::Type::Material; };

	private:
		MaterialTemplate m_Template;
		bgfx::ProgramHandle m_ProgramHandle;
		std::unordered_map<StringName, bgfx::UniformHandle> m_UniformHandles;
	};

	class MaterialInstance
	{
	public:
		MaterialInstance(Ref<Material> material);

		void SetParam(const StringName& name, const glm::vec4& value) { m_Values[name] = value; }
		const std::variant<Vec4, Ref<Texture>>& GetParam(const StringName& name) const { return m_Values.at(name); }

		void Bind();

	private:
		Ref<Material> m_Material;
		std::unordered_map<StringName, std::variant<Vec4, Ref<Texture>>> m_Values;
	};
}