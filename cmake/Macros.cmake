function(BUILD_EXAMPLE EXPNAME)
  set(options)
  set(oneValueArgs FOLDER)
  set(multiValueArgs)
  cmake_parse_arguments(BUILD_EXAMPLE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT BUILD_EXAMPLE_FOLDER)
    message(FATAL_ERROR "BUILD_EXAMPLE needs FOLDER_NAME property!")
  endif()

  message(STATUS "${BUILD_EXAMPLE_FOLDER}/${EXPNAME}")
  set(TARGET_EXAMPLE_DIR "${CMAKE_CURRENT_LIST_DIR}/${EXPNAME}")

  if (EXISTS "${TARGET_EXAMPLE_DIR}/common.h.in")
    configure_file("${TARGET_EXAMPLE_DIR}/common.h.in"
                   "${TARGET_EXAMPLE_DIR}/common.h" @ONLY)
  endif()

  file(GLOB SOURCE_FILES "${EXPNAME}/*.cpp" "${EXPNAME}/*.h")
  file(GLOB SHADER_FILES "${EXPNAME}/shaders/*.vert" "${EXPNAME}/shaders/*.frag")

  include_directories(${ALL_INCLUDE_DIRS})
  add_executable(${EXPNAME} ${SOURCE_FILES} ${SHADER_FILES} ${GLAD_FILES})
  target_link_libraries(${EXPNAME} ${ALL_LIBRARIES})

  set_target_properties(${EXPNAME} PROPERTIES FOLDER ${BUILD_EXAMPLE_FOLDER})

  source_group("Source Files" FILES ${SOURCE_FILES})
  source_group("Shader Files" FILES ${SHADER_FILES})
endfunction(BUILD_EXAMPLE)