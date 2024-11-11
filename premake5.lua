local proj = "cmap"

workspace (proj)
    configurations { "Debug", "Release" }
    platforms { "x64", "x86" }

project (proj)
  kind "ConsoleApp"
	language "C++"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"

	files {
	   "~depositories/jsoncpp/src/lib_json/*.cpp",
	   "~depositories/jsoncpp/src/lib_json/*.h",

	   "cfg/**",
	   "cli/**",
	   "fs/**",
	   "repo/**",
	   "utils/**",

	   "config.h",
	   "main.cpp"
	}

	externalincludedirs { "~depositories/jsoncpp/include" }
	buildoptions { "/std:c++latest" }
	characterset "MBCS"
	warnings "Off"
	optimize "Size"