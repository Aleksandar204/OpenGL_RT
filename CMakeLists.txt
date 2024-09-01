cmake_minimum_required(VERSION 3.16)

project("EngineRT")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)


include_directories("./include")

file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c")

add_executable(engineRT ${SRC_FILES})

target_link_libraries(engineRT OpenGL assimp glfw)