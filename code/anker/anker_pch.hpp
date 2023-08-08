#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <ranges>
#include <span>
#include <stack>
#include <system_error>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#if _WIN32
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

#include <dxgi1_3.h>
#pragma comment(lib, "dxguid.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#undef near
#undef far
#endif // _WIN32

#include <refl.hpp>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include <entt/entt.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_glfw.h>
#include <imgui_stdlib.h>

// #include <ImGuizmo.h>

////////////////////////////////////////////////////////////

namespace Anker {

using namespace entt::literals;

// Recent MSVC workaround
using refl::descriptor::has_attribute;

} // namespace Anker

#include <anker/common/anker_common.hpp>
