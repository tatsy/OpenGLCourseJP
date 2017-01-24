if (WIN32)

else()
  find_path(GLM_INCLUDE_DIR
            NAMES glm/glm.hpp
            PATHS
            /usr/include
            /usr/local/include)
endif()

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

