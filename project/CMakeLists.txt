#Source files
set(SOURCES
	"src/main.cpp"
)

#Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

#add glm library
include_directories("libs/glm")

#add glfw
add_subdirectory(libs/glfw-3.4)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

#add Vulkan
find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})