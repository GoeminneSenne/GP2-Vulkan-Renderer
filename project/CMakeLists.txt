#Source files
set(SOURCES
	"src/main.cpp"
)

#Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

#Enable C++20 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

#Fetch required packages
include(FetchContent)

#add glm library
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
	FetchContent_Declare(
		glm
		URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
		DOWNLOAD_NO_PROGRESS ON
		DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
	)
	FetchContent_MakeAvailable(glm)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glm)

#add glfw
find_package(glfw CONFIG QUIET)
if(NOT glfw_FOUND)
	FetchContent_Declare(
		glfw
		URL https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
		DOWNLOAD_NO_PROGRESS ON
		DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
	)
	FetchContent_MakeAvailable(glfw)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

#add Vulkan
find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})