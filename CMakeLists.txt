cmake_minimum_required(VERSION 2.8.0)
project(OpenGLCourseJP)

if (WIN32)
  set(GLEW_ROOT "GLEW_ROOT" CACHE PATH "")
  set(GLFW3_ROOT "GLFW3_ROOT" CACHE PATH "")
  set(GLM_ROOT "GLM_ROOT" CACHE PATH "")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# ------------------------------------------------------------------------------
# Set build options
# ------------------------------------------------------------------------------
if (NOT MSVC)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -std=c++11")
endif()

# ------------------------------------------------------------------------------
# Find OpenGL libraries
# ------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

# ------------------------------------------------------------------------------
# Set output path
# ------------------------------------------------------------------------------
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ------------------------------------------------------------------------------
# Setup dependencies
# ------------------------------------------------------------------------------
set(ALL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS}
                     ${GLFW3_INCLUDE_DIR}
                     ${GLEW_INCLUDE_DIR}
                     ${GLM_INCLUDE_DIR})

set(ALL_LIBRARIES ${OPENGL_LIBRARIES} ${GLFW3_LIBRARY} ${GLEW_LIBRARY})
if (UNIX AND NOT APPLE)
  find_package(PkgConfig REQUIRED)
  pkg_search_module(GLFW3 REQUIRED glfw3)
  set(ALL_LIBRARIES ${ALL_LIBRARIES} ${CMAKE_DL_LIBS} ${GLFW3_STATIC_LIBRARIES})
endif()

if (APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework IOKit -framework CoreVideo")
endif()

# ------------------------------------------------------------------------------
# Add subdirectory
# ------------------------------------------------------------------------------
add_subdirectory(src)
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/practice/CMakeLists.txt")
  add_subdirectory(practice)
endif()
