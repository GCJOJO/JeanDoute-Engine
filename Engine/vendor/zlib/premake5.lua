project "zlib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++14"
	staticruntime "off"

    targetdir ("%{prj.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{prj.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "*.c",
        "*.rc",
        "*.def",
        "*.res",
    }

    filter "Windows"
        systemversion "latest"

        defines
        {
            "ZLIB_WINAPI",
            "WIN64"
        }