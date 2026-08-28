project "minizip"
    kind "StaticLib"
    language "C"
	staticruntime "off"

    targetdir ("%{prj.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{prj.location}/bin-int/" .. outputdir .. "/%{prj.name}")


    includedirs
    {
        "*.h"
    }

    files
    {
        "*.h",
        "*.c",
    }

    includedirs
    {
        "%{IncludeDir.zlib}"
    }

    links
    {
        "zlib"
    }