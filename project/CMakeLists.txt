#Framework library
set(FRAMEWORK_SOURCES
	src/stb_image.cpp
	src/framework/Window.cpp
	src/framework/Instance.cpp
	src/framework/PhysicalDevice.cpp 
	src/framework/LogicalDevice.cpp
	src/framework/Swapchain.cpp
	src/framework/Image.cpp
	src/framework/Shader.cpp
	src/framework/Sampler.cpp
	
	src/framework/Queue.cpp
	src/framework/RenderPass.cpp
	src/framework/Context.cpp
	src/framework/CommandBuffer.cpp
	src/framework/DescriptorSet.cpp
	src/framework/GraphicsPipeline.cpp
	src/framework/PipelineLayout.cpp
	src/framework/SwapchainSupportDetails.cpp
	src/framework/Framebuffer.cpp
	src/framework/Buffer.cpp
	src/framework/Mesh.cpp
	src/framework/SyncManager.cpp
	src/framework/GLTFLoader.cpp
	src/framework/Camera.cpp)

add_library(Vulkan_Framework STATIC ${FRAMEWORK_SOURCES})
target_compile_features(Vulkan_Framework PUBLIC cxx_std_20)
target_include_directories(Vulkan_Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

#Executable
set(SOURCES
	src/application/main.cpp
	src/application/VulkanDemoApplication.cpp
	src/application/SponzaApplication.cpp
	src/application/RenderPassExample.cpp)

#Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

#Enable C++20 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

#Link framework library
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan_Framework)

#Fetch required packages
include(FetchContent)

#add glm library
FetchContent_Declare(
	glm
	URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
	DOWNLOAD_NO_PROGRESS ON
	DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
)
FetchContent_MakeAvailable(glm)



#add glfw
FetchContent_Declare(
	glfw
	URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
	DOWNLOAD_NO_PROGRESS ON
	DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
)
FetchContent_MakeAvailable(glfw)



#add Vulkan
find_package(Vulkan REQUIRED)

# Vulkan Memory Allocator
FetchContent_Declare(
	VulkanMemoryAllocator
	URL https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator/archive/refs/tags/v3.3.0.zip
	DOWNLOAD_NO_PROGRESS ON
	DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

# Assimpµ
set(CMAKE_DISABLE_FIND_PACKAGE_Doxygen TRUE) 
FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	GIT_TAG v6.0.5
	GIT_SHALLOW TRUE
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE) 
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

target_link_libraries(Vulkan_Framework PUBLIC
	glm
	glfw
	${Vulkan_LIBRARIES}
	GPUOpen::VulkanMemoryAllocator
	assimp
)

target_include_directories(Vulkan_Framework PUBLIC ${Vulkan_INCLUDE_DIRS})

# STB
#Fetchcontent van stb

#target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR}
# en dan includen in project
#best specifieke verion downloaden
# GIT_SHALLOW TRUE


# --- DEFINE SHADER COMPILATION AS BUILDING STEP OF PROJECT ---
file(GLOB_RECURSE SHADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/*.frag"
	"${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/*.vert"
	"${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/*.comp"
)
message("Found shaders: ${SHADERS}")
message("Source dir: ${CMAKE_SOURCE_DIR}")

set(SPIRV_FILES)
foreach(shader ${SHADERS})
	# Get the relative path from the shaders directory
	file(RELATIVE_PATH relPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders ${shader})

	# Append .spv extension
	set(outputFile "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/${relPath}.spv")

	# Ensure the output directory exists
	get_filename_component(outputDir ${outputFile} DIRECTORY)
	file(MAKE_DIRECTORY ${outputDir})

	# Add command to compile -> (-g flag is to include debug info)
	add_custom_command(
		OUTPUT ${outputFile}
		COMMAND glslangValidator -V -g ${shader} -I${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/ -o ${outputFile}
		DEPENDS ${shader}
		COMMENT "Compiling GLSL shader ${filename} to SPIR-V"
		VERBATIM
	)
	
	list(APPEND SPIRV_FILES ${outputFile}) #add files for dependency
endforeach()

# Add a custom target that depends on all the SPIR-V files and where our Vulkan
# project depends on shader compilation
add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_FILES})
add_dependencies(${PROJECT_NAME} CompileShaders)

# Custom target to copy resources folder
#add_custom_target(CopyResources ALL DEPENDS CompileShaders
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#	"${CMAKE_CURRENT_SOURCE_DIR}/resources"
#	"$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
#	COMMENT "Copying resources folder to build directory")


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
    COMMENT "Copying resources folder to build directory"
)