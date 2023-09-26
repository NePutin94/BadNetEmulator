cmake_minimum_required(VERSION 3.25)
project(BadNetEmulator)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.89.6
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG "3.3.5"
)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(BUILD_SHARED_LIBS ON)
FetchContent_MakeAvailable(glfw)

set(FMT_DOC OFF)
set(FMT_TEST OFF)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG "0.9.9.8"
)
FetchContent_MakeAvailable(glm)

file(GLOB CORE_SRC RELATIVE "${CMAKE_SOURCE_DIR}/" "src/*.cpp")
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
)
file(GLOB IMGUI_BACKEND RELATIVE "${CMAKE_SOURCE_DIR}/" "lib/imgui/*.cpp")
find_package(OpenGL REQUIRED)

add_executable(BadNetEmulator ${CORE_SRC} ${IMGUI_BACKEND} lib/glad/src/gl.c main.cpp)

target_link_libraries(BadNetEmulator PRIVATE fmt::fmt glfw imgui glm::glm)

target_include_directories(BadNetEmulator PRIVATE ${OPENGL_LIBRARIES} ${imgui_SOURCE_DIR} lib/glad/include lib/imgui)

add_custom_command(TARGET BadNetEmulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:glfw> $<TARGET_FILE_DIR:BadNetEmulator>)