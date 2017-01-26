if (WIN32)
  find_path(GLM_INCLUDE_DIR
            NAMES glm/glm.hpp
            PATHS
            ${GLM_ROOT}/include)
else()
  find_path(GLM_INCLUDE_DIR
            NAMES glm/glm.hpp
            PATHS
            /usr/include
            /usr/local/include)
endif()

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)
