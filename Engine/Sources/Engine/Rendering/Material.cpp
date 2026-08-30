#include "enginepch.h"
#include "Engine/Rendering/Material.h"
#include "Engine/Core/Assert.h"
#include "Engine/Assets/AssetImporter.h"
#include "Engine/Assets/AssetManager.h"

#include <regex>

// Merci Claude Slop
namespace Engine
{
	MaterialParamDef::ParamType ParseTypeKeyword(const std::string& keyword)
	{
		if (keyword == "float")		return MaterialParamDef::ParamType::Float;
		if (keyword == "vec4")		return MaterialParamDef::ParamType::Vec4;
		if (keyword == "color")		return MaterialParamDef::ParamType::Color;
		if (keyword == "texture")	return MaterialParamDef::ParamType::Texture;

		ENGINE_CORE_ASSERT(false, "Invalid shader parameter type " + keyword);
	}

	std::string StripQuotes(const std::string& str)
	{
		if (str.size() >= 2 && str.front() == '"' && str.back() == '"')
			return str.substr(1, str.size() - 2);
		return str;
	}

	Vec4 ParseVec4(const std::string& str)
	{
		// Accepte "1,1,1,1" (vec4/color) ou "0.5" (float, packé sur x)
		glm::vec4 result(0.0f);
		std::stringstream ss(str);
		std::string component;
		int i = 0;
		while (std::getline(ss, component, ',') && i < 4)
		{
			result[i] = std::stof(component);
			++i;
		}
		return result;
	}

	void ParseParamLine(const std::string& line, MaterialParamDef& def, bool isTexture = false)
	{
		static const std::regex tokenRegex(R"((\w+)=("[^"]*"|[^\s]+)|(\w+))");

		auto begin = std::sregex_iterator(line.begin(), line.end(), tokenRegex);
		auto end = std::sregex_iterator();
		
		if (isTexture)
			def.Type = MaterialParamDef::ParamType::Texture;

		bool first = true;
		bool readUniformName = false;

		for (auto& it = begin; it != end; it++)
		{
			const std::smatch& match = *it;
			
			std::string wholeMatch = match[0];

			if (first)
			{
				first = false;
				continue;
			}

			if (!readUniformName)
			{
				def.UniformName = wholeMatch;
				readUniformName = true;
				continue;
			}

			if (match[1].matched)
			{
				std::string key = match[1];
				std::string value = StripQuotes(match[2]);
				if (key == "min") def.Min = std::stof(value);
				else if (key == "max") def.Max = std::stof(value);
				else if (key == "default")
				{
					if (def.Type == MaterialParamDef::ParamType::Texture)
						def.DefaultValue = AssetManager::GetAsset<Texture>(value);
					else
						def.DefaultValue = ParseVec4(value);
				}
				else if (key == "label") def.Label = value;
			}
			else if (match[3].matched)
			{
				def.Type = ParseTypeKeyword(match[3]);
			}
		}
	}

	Material::Material(MaterialTemplate materialTemplate, bgfx::ProgramHandle program)
		: m_Template(materialTemplate), m_ProgramHandle(program)
	{
		for (const auto& param : m_Template.params)
		{
			if (param.Type == MaterialParamDef::ParamType::Texture)
				continue;

			if (!m_UniformHandles.contains(param.UniformName))
				m_UniformHandles[param.UniformName] = bgfx::createUniform(
					param.UniformName.c_str(), bgfx::UniformType::Vec4);
		}
	}

	MaterialInstance::MaterialInstance(Ref<Material> material)
		: m_Material(material)
	{
		for (const auto& param : material->GetTemplate().params)
			m_Values[param.UniformName] = param.DefaultValue;
	}

	void MaterialInstance::Bind()
	{
		for (const auto& [name, value] : m_Values)
			bgfx::setUniform(m_Material->GetUniform(name), &value);
	}

	MaterialTemplate MaterialTemplate::ParseMaterialTemplate(const std::filesystem::path& sourcePath)
	{
		MaterialTemplate materialTemplate;
		materialTemplate.shaderPath = sourcePath;

		std::ifstream file(sourcePath);
		if (!file.is_open())
		{
			ENGINE_CORE_ASSERT(false, "Unable to open shader " + sourcePath.string());
			return;
		}

		std::string line;
		while (std::getline(file, line))
		{
			bool isParam	= line.starts_with("// @param");
			bool isTexutre	= line.starts_with("// @texture");

			if (!isParam && !isTexutre)
				continue;

			MaterialParamDef def;
			ParseParamLine(line, def, isTexutre);
			materialTemplate.params.push_back(std::move(def));
		}
		return materialTemplate;
	}

}