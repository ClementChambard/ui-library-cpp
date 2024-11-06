cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "Debug")

add_compile_options(-Wall -Wextra -Werror)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(-O4)
endif()

project("uilib" VERSION 0.1)

file(GLOB_RECURSE SRCS src/*.cpp src/*.c)
file(GLOB_RECURSE HEADERS src/*.hpp src/*.h)

add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
set(SDL2_LIBS -lSDL2)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARIES} ${SDL2_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED True)
