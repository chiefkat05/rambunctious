cmake_minimum_required(VERSION 3.10)

project(main)

add_executable(main ${CMAKE_SOURCE_DIR}/sources/main.cpp ${CMAKE_SOURCE_DIR}/sources/sprite.cpp)

if (LINUX)
target_link_libraries(main sfml-audio sfml-graphics sfml-window sfml-system)
endif()

if (WIN32)
target_link_libraries(main sfml-audio sfml-graphics sfml-window sfml-system opengl32 openal32)
endif()

set(CMAKE_CXX_FLAGS "-O2")