-- JeanDoute-Engine Dependencies --

VULKAN_SDK = os.getenv("VULKAN_SDK")
printf("Vulkan SDK: %s", VULKAN_SDK)

IncludeDir = {}
IncludeDir["Assimp"] = "%{wks.location}/Engine/vendor/Assimp/include"
IncludeDir["bgfx"] = "%{wks.location}/Engine/vendor/bgfx/include"
IncludeDir["bimg"] = "%{wks.location}/Engine/vendor/bimg/include"
IncludeDir["bx"] = "%{wks.location}/Engine/vendor/bx/include"
IncludeDir["coral_native"] = "%{wks.location}/Engine/vendor/coral/Coral.Native/include"
IncludeDir["curl_lib"] = "%{wks.location}/Engine/vendor/curl/include"
IncludeDir["filewatch"] = "%{wks.location}/Engine/vendor/filewatch"
IncludeDir["Glad"] = "%{wks.location}/Engine/vendor/Glad/include"
IncludeDir["GLFW"] = "%{wks.location}/Engine/vendor/GLFW/include"
IncludeDir["glm"] = "%{wks.location}/Engine/vendor/glm"
IncludeDir["ImGui"] = "%{wks.location}/Engine/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Engine/vendor/ImGuizmo"
IncludeDir["libcurl"] = "%{wks.location}/Engine/vendor/curl/include"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/Engine/vendor/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "%{wks.location}/Engine/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["minizip"] = "%{wks.location}/Engine/vendor/zlib/contrib/minizip"
IncludeDir["shaderc"] = "%{wks.location}/Engine/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Engine/vendor/SPIRV-Cross"
IncludeDir["stb_image"] = "%{wks.location}/Engine/vendor/stb_image"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Engine/vendor/yaml-cpp/include"
IncludeDir["zlib"] = "%{wks.location}/Engine/vendor/zlib"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["Coral_Managed"] = "%{wks.location}/Engine/vendor/coral/Coral.Managed/Build/%{cfg.buildcfg}/net9.0/"
LibraryDir["libcurl"] = "%{wks.location}/Engine/vendor/curl/lib"
LibraryDir["libcurl_dynamic"] = "%{wks.location}/Engine/vendor/curl/bin"

Library = {}

Library["libcurl"] = "%{LibraryDir.libcurl}/libcurl.lib"
Library["libbrotlicommon"] = "%{LibraryDir.libcurl}/libbrotlicommon.lib"
Library["libbrotlidec"] = "%{LibraryDir.libcurl}/libbrotlidec.lib"
Library["libcrypto"] = "%{LibraryDir.libcurl}/libcrypto.lib"
Library["libnghttp2"] = "%{LibraryDir.libcurl}/libnghttp2.lib"
Library["libnghttp3"] = "%{LibraryDir.libcurl}/libnghttp3.lib"
Library["libngtcp2"] = "%{LibraryDir.libcurl}/libngtcp2.lib"
Library["libngtcp2_crypto_libressl"] = "%{LibraryDir.libcurl}/libngtcp2_crypto_libressl.lib"
Library["libpsl"] = "%{LibraryDir.libcurl}/libpsl.lib"
Library["libssh2"] = "%{LibraryDir.libcurl}/libssh2.lib"
Library["libssl"] = "%{LibraryDir.libcurl}/libssl.lib"
Library["libz"] = "%{LibraryDir.libcurl}/libz.lib"
Library["libzstd"] = "%{LibraryDir.libcurl}/libzstd.lib"

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"


-- Windows

Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"

--DynamicLibrary = {}
DynamicLibrary = {}
DynamicLibrary["Coral_Managed"] = "%{LibraryDir.Coral_Managed}/Coral.Managed.dll"
--DynamicLibrary["libcurl-win"] = "%{LibraryDir.libcurl_dynamic}/libcurl-x64.dll"