include(FindPackageHandleStandardArgs)

set(GLEW_DIR "" CACHE PATH "")

find_path(GLEW_INCLUDE_DIR
        NAMES GL/glew.h
        PATHS
        /usr/include
        /usr/local/include
        /usr/local/Cellar/glew/include
        ${GLEW_DIR}/include)

find_library(GLEW_LIBRARY
        NAMES glew glew32 GLEW
        PATHS
        /usr/lib
        /usr/local/lib
        /usr/local/Cellar/glew/lib
        /usr/lib/x86_64-linux-gnu
        ${GLEW_DIR}/lib
        ${GLEW_DIR}/lib/Release/Win32
        ${GLEW_DIR}/lib/Release/x64)

find_package_handle_standard_args(
    GLEW
    DEFAULT_MSG
    GLEW_INCLUDE_DIR
    GLEW_LIBRARY
)

if (GLEW_FOUND)
    message(STATUS "GLEW include: ${GLEW_INCLUDE_DIR}")
    message(STATUS "GLEW library: ${GLEW_LIBRARY}")
    set(GLEW_INCLUDE_DIRS "${GLEW_INCLUDE_DIR}" CACHE PATH "")
    set(GLEW_LIBRARIES "${GLEW_LIBRARY}" CACHE FILEPATH "")
    mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)
endif()
