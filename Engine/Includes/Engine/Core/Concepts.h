#pragma once

#include <concepts>

namespace Engine
{
    class Node;

    template<typename T>
	concept DerivedFromNode = std::derived_from<T, Node>;
}