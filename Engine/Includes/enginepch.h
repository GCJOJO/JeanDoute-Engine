#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Engine/Core/Assert.h"
#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
#include "Engine/Core/Buffer.h"
#include "Engine/Core/Profiler.h"
#include "Engine/Core/UUID.h"

#ifdef ENGINE_PLATFORM_WINDOWS
#include <Windows.h>
#endif