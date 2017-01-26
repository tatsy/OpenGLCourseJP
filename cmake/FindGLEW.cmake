if (WIN32)
  find_path(GLEW_INCLUDE_DIR
            NAMES GL/glew.h
            PATHS
            ${GLEW_ROOT}/include)

  find_library(GLEW_LIBRARY
               NAMES glew32.lib
               PATHS
               ${GLEW_ROOT}/lib)
else()
  find_path(GLEW_INCLUDE_DIR
            NAMES GL/glew.h
            PATHS
            /usr/include
            /usr/local/include)

  find_library(GLEW_LIBRARY
               NAMES libGLEW.so libGLEW.a
               PATHS
               /usr/lib
               /usr/local/lib
               /usr/lib/x86_64-linux-gnu)
endif()

find_package_handle_standard_args(GLEW DEFAULT_MSG GLEW_INCLUDE_DIR GLEW_LIBRARY)
