#pragma once

#include "Engine/Types/Resources/Script.h"

namespace Engine
{
	using ScriptState = std::unordered_map<StringName, Variant>;

	class ScriptInstance
	{
	public:
		virtual ~ScriptInstance() = default;

		virtual Ref<Script> GetScript() const = 0;
		virtual Node* GetOwner() const = 0;

		virtual bool HasMethod(const StringName& method) const = 0;
		virtual bool Call(const StringName& method, Variant* args, std::size_t argCount, Variant& result) = 0;
		virtual bool Get(const StringName& property, Variant& out) const = 0;
		virtual bool Set(const StringName& property, const Variant& value) = 0;
		virtual bool GetPropertyList(std::vector<ScriptPropertyInfo>& out) const = 0;

		virtual void Notification(int what) = 0;

		virtual ScriptState SaveState() const = 0;
		virtual void RestoreState(const ScriptState& state) = 0;
	};
}