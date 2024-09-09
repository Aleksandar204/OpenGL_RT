cmake_minimum_required(VERSION 3.16)

project("EngineRT")

find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

include_directories("./include")

file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.c")

add_executable(engineRT ${SRC_FILES})

target_link_libraries(engineRT assimp::assimp glfw)

file(GLOB SHADER_FILES "${CMAKE_SOURCE_DIR}/shaders/*")

foreach(FILE ${SHADER_FILES})
        get_filename_component(FILENAME ${FILE} NAME)
        configure_file(${FILE} ${CMAKE_BINARY_DIR}/resources/shaders/${FILENAME} COPYONLY)
endforeach()

add_custom_command(
        TARGET engineRT POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/resources/shaders
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/resources/models
)