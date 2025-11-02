cmake_minimum_required(VERSION 3.20)
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
project(BastauEduGraph)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ---- Dependencies ----
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)




if (MSVC)
    add_definitions(-DNOMINMAX)
endif()

# ---- ImGui directory ----
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/src/vendor/imgui")

# ---- Executable ----
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/app.cpp
        src/core/graph.cpp
        src/renderer/graph_renderer.cpp
        src/modules/control/controller_panel.h
        src/modules/control/worker_panel.h
        src/modules/control/message_bus.h
        # ❗ message_bus.cpp енді қажет емес, CMake-тен алып тастадық
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ---- Include directories ----
target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/vendor
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        C:/vcpkg/installed/x64-windows/include
)

# ---- Defines for ImGui backend with GLAD ----
target_compile_definitions(${PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# ---- Link libraries ----
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glfw
        glad::glad
        glm::glm
        OpenGL::GL
)
