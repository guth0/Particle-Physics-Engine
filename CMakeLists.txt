cmake_minimum_required(VERSION 3.12)
project(PhysicsEngine)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics REQUIRED)

add_executable(main main.cpp head.h system.h renderer.h)

# Include SFML headers
target_include_directories(main PRIVATE ${SFML_INCLUDE_DIR})

target_link_libraries(main sfml-graphics)
