cmake_minimum_required(VERSION 3.27)
project(bakermakersetuper)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(libs/glfw)

set(LIBROMFS_PROJECT_NAME bakermakersetuper)
set(LIBROMFS_RESOURCE_LOCATION ${PROJECT_SOURCE_DIR}/resources)
add_subdirectory(libs/libromfs)

set(LIBZIP_DO_INSTALL OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(libs/miniz)


add_executable(bakermakersetuper src/main.cpp libs/imgui/imgui.cpp libs/imgui/imgui_draw.cpp
        libs/imgui/imgui_tables.cpp libs/imgui/imgui_widgets.cpp
        libs/imgui/backends/imgui_impl_glfw.cpp libs/imgui/backends/imgui_impl_opengl3.cpp
        src/libs/imfilebrowser.cpp
        src/ThreadedExtractor.cpp
        src/libs/improgress.cpp windowsrc/icon.rc
        src/libs/IconSetter.cpp)

target_link_libraries(bakermakersetuper PUBLIC glfw opengl32 gdi32 imm32 ${LIBROMFS_LIBRARY} miniz)
target_include_directories(bakermakersetuper PUBLIC libs/glfw/include libs/imgui
        libs/imgui/backends libs/miniz include)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "gitclient")