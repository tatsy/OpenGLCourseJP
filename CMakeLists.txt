cmake_minimum_required(VERSION 2.8.0)
project(OpenGLCourse)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# ------------------------------------------------------------------------------
# Set build options
# ------------------------------------------------------------------------------
if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

# ------------------------------------------------------------------------------
# Find OpenGL
# ------------------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# ------------------------------------------------------------------------------
# Find GLFW3
# ------------------------------------------------------------------------------
if (MSVC)
  set(GLFW3_INCLUDE_DIR "GLFW3_INCLUDE_DIR" CACHE PATH "")
  set(GLFW3_LIBRARY "GLFW3_LIBRARY" CACHE PATH "")
else()
  find_package(GLFW3 REQUIRED)
endif()

# ------------------------------------------------------------------------------
# Find GLEW
# ------------------------------------------------------------------------------
if (MSVC)
  set(GLEW_INCLUDE_DIR "GLEW_INCLUDE_DIR" CACHE PATH "")
  set(GLEW_LIBRARY "GLEW_LIBRARY" CACHE PATH "")
else()
  find_package(GLEW REQUIRED)
endif()


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
                     ${GLEW_INCLUDE_DIR})

set(ALL_LIBRARIES ${OPENGL_LIBRARIES} ${GLFW3_LIBRARY} ${GLEW_LIBRARY})

# ------------------------------------------------------------------------------
# Add subdirectory
# ------------------------------------------------------------------------------
add_subdirectory(src)
