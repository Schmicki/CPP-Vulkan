workspace "VulkanEngine"
	architecture "x86_64"
	configurations {
		"DebugEditor-x64",
		"ReleaseEditor-x64",
		"DebugGame-x64",
		"ReleaseGame-x64",
	}
	
    startproject "VulkanEngine"

	filter {"action:vs*"}
		platforms { "Windows" }
    	defaultplatform "Windows"

	filter {"action:gmake*"}
		platforms { "Linux" }
    	defaultplatform "Linux"

	filter "configurations:*Debug*"
		defines {"DEBUG"}
		symbols "On"

	filter "configurations:*Release*"
		defines {"NDEBUG"}
		optimize "On"

	filter "configurations:*Editor*"
		defines {"SGE_WITH_EDITOR"}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

	filter "system:linux"
		cppdialect "C++17"
	
	filter { "system:linux", "action:gmake" }
		buildoptions { "-fPIC" }


project "Types"
	location "Source/Engine/Types"
	kind "StaticLib"
	language "C++"

	dependson {
		"SGECodeGen"
	}

	defines {"_TYPES_"}

	targetdir("Bin/%{cfg.buildcfg}/")
	objdir("Intermediate/%{cfg.buildcfg}/")

	files {
		"Source/Engine/%{prj.name}/**.h",
		"Source/Engine/%{prj.name}/**.cpp"
	}

	includedirs {
		"ThirdParty/stb/",
		"ThirdParty/SadFileFormat/",
		"Source/Engine/Types"
	}

	filter {"configurations:*Debug*", "system:windows"}
  		--prebuildcommands
		--{
		--	"../../../build -codegen debug"
		--}
	
	filter {"configurations:*Release*", "system:windows"}
		--prebuildcommands
	  	--{
		--	"../../../build -codegen release"
	  	--}

	filter {"configurations:*Debug*", "system:linux"}
  		--prebuildcommands
		--{
		--	"../../../build.sh -codegen debug"
		--}
	
	filter {"configurations:*Release*", "system:linux"}
		--prebuildcommands
	  	--{
		--	"../../../build.sh -codegen release"
	  	--}


project "Window"
	location "Source/Engine/Window"
	kind "StaticLib"
	language "C++"

	defines {"_WINDOW_"}

	links {
		"Types"
	}

	targetdir("Bin/%{cfg.buildcfg}/")
	objdir("Intermediate/%{cfg.buildcfg}/")

	files {
		"Source/Engine/%{prj.name}/**.h",
		"Source/Engine/%{prj.name}/**.cpp"
	}

	includedirs {
		"Source/Engine"
	}

	filter "platforms:Linux"
		includedirs {"/usr/include/xcb"}
		libdirs {"/usr/bin"}
    	links {"xcb"}


project "Interfaces"
	location "Source/Engine/Interfaces"
	kind "StaticLib"
	language "C++"
	
	targetdir("Bin/%{cfg.buildcfg}/")
	objdir("Intermediate/%{cfg.buildcfg}/")
	
	files {
		"Source/Engine/%{prj.name}/**.h",
		"Source/Engine/%{prj.name}/**.cpp"
	}


project "VulkanRI"
	location "Source/Engine/VulkanRI/"
	kind "SharedLib"
	language "C++"
	
	targetdir("Bin/%{cfg.buildcfg}/")
	objdir("Intermediate/%{cfg.buildcfg}/")
	
	defines {"_VulkanRI"}
	
	files {
		"Source/Engine/%{prj.name}/**.h",
		"Source/Engine/%{prj.name}/**.cpp"
	}

	VK_PATH = ""
	if VK_PATH == "" then VK_PATH = os.getenv("VULKAN_SDK") end
	if VK_PATH == "" then VK_PATH = os.getenv("VK_SDK_PATH") end

	includedirs {
		"%{VK_PATH}/Include",
		"/usr/include/vulkan",
		"Source/Engine",
	}
	
	libdirs { "%{VK_PATH}/Lib" , "/usr/lib/x86_64-linux-gnu" }
	
	links {
		"Window",
		"Types"
	}

	filter "platforms:Linux"
		links{"vulkan"}


	filter "platforms:Windows"
		links {"vulkan-1"}
	
	
project "VulkanEngine"
	location "Source/Engine/VulkanEngine"
	kind "ConsoleApp"
	language "C++"

	targetdir("Bin/%{cfg.buildcfg}/")
	objdir("Intermediate/%{cfg.buildcfg}/")

	debugdir "Source/"

	links {
		"Types",
		"Window",
		"Interfaces",
		"VulkanRI"
	}

	files {
		"Source/Engine/%{prj.name}/**.h",
		"Source/Engine/%{prj.name}/**.cpp",
	}
	
	includedirs {
		"Source/Engine"
	}

	filter "platforms:Linux"
		includedirs {"/usr/include/xcb"}
		libdirs {"/usr/bin"}
    	links {"xcb"}


project "SGECodeGen"
	location "Source/Tools/SGECodeGen"
	kind "ConsoleApp"
	language "C++"
	
	files {
		"Source/Tools/SGECodeGen/Source/**.h",
		"Source/Tools/SGECodeGen/Source/**.cpp"
	}
	
	includedirs {
		"Source/Engine",
	}

	filter "configurations:*Debug*"
		targetdir("Source/Tools/SGECodeGen/Bin/Debug/")
		objdir("Source/Tools/SGECodeGen/Intermediate/Debug/")

	filter "configurations:*Release*"
		targetdir("Source/Tools/SGECodeGen/Bin/Release/")
		objdir("Source/Tools/SGECodeGen/Intermediate/Release/")