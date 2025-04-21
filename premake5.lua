local proj = "cmap"

workspace (proj)
    configurations { "Debug", "Release" }
    platforms { "x64", "x86" }

project (proj)
  kind "ConsoleApp"
	language "C++"
	location "src"

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"

	files {
	   "src/dependencies/jsoncpp/src/lib_json/*.cpp",
	   "src/dependencies/jsoncpp/src/lib_json/*.h",

	   "src/cfg/**",
	   "src/cli/**",
	   "src/fs/**",
	   "src/repo/**",
	   "src/utils/**",

	   "src/config.h",
	   "src/main.cpp"
	}

	externalincludedirs { "src/dependencies/jsoncpp/include" }
	buildoptions { "/std:c++latest" }
	characterset "MBCS"
	warnings "Off"
	optimize "Size"