cmake_minimum_required(VERSION 3.21)
project(Bastau3DLiveCells CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Vcpkg қолдансаңыз, VS Code-тан генерацияда -DCMAKE_TOOLCHAIN_FILE=... берілген болсын
# Пакеттер: glfw3, glad, glm
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/core/node.cpp
    src/core/edge.cpp
    src/core/graph.cpp
    src/renderer/node_renderer.cpp
    src/renderer/graph_renderer.cpp
    src/utils/shader_loader.cpp
    src/utils/camera.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glad::glad glm::glm)

# Шейдерлерді орындалатын файл қасына көшіреміз
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
