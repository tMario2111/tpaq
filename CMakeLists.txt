cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tpaq)

add_executable(tpaq main.cpp)

find_package(SFML COMPONENTS graphics system CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")

target_link_libraries(tpaq PRIVATE 
    sfml-graphics sfml-system
    ${OPENGL_LIBRARIES}
    nlohmann_json::nlohmann_json
    CLI11::CLI11)

target_include_directories(tpaq PRIVATE ${STB_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
