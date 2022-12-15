cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

project(tpaq_test)

add_executable(tpaq_test main.cpp
    source/pack.cpp

    ../source/TexturePacker.hpp ../source/TexturePacker.cpp
    
    ../mustache.hpp)

find_package(doctest CONFIG REQUIRED)
find_package(SFML COMPONENTS graphics system CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")

target_link_libraries(tpaq_test PRIVATE
    doctest::doctest
    sfml-graphics sfml-system
    ${OPENGL_LIBRARIES}
    CLI11::CLI11)

target_include_directories(tpaq_test PRIVATE ${STB_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
