if (WIN32)
  find_path(GLFW3_INCLUDE_DIR
            NAMES GLFW/glfw3.h
            PATHS
            ${GLFW3_ROOT}/include)

  find_library(GLFW3_LIBRARY
               NAMES glfw3.lib
               PATHS
               ${GLFW3_ROOT}/lib)
else()
  find_path(GLFW3_INCLUDE_DIR
            NAMES GLFW/glfw3.h
            PATHS
            /usr/include
            /usr/local/include)

  find_library(GLFW3_LIBRARY
               NAMES libglfw3.a
               PATHS
               /usr/lib
               /usr/local/lib
               /usr/lib/x86_64-linux-gnu)
endif()

find_package_handle_standard_args(GLFW3 DEFAULT_MSG GLFW3_INCLUDE_DIR GLFW3_LIBRARY)
