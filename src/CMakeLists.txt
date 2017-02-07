function(BUILD_EXAMPLE EXPNAME)
  set(TARGET_EXAMPLE_DIR "${CMAKE_CURRENT_LIST_DIR}/${EXPNAME}")

  if (EXISTS "${TARGET_EXAMPLE_DIR}/common.h.in")
    configure_file("${TARGET_EXAMPLE_DIR}/common.h.in"
                   "${TARGET_EXAMPLE_DIR}/common.h" @ONLY)
  endif()

  file(GLOB SOURCE_FILES "${EXPNAME}/*.cpp" "${EXPNAME}/*.h")
  file(GLOB SHADER_FILES "${EXPNAME}/shaders/*.vert" "${EXPNAME}/shaders/*.frag")

  include_directories(${ALL_INCLUDE_DIRS})
  add_executable(${EXPNAME} ${SOURCE_FILES} ${SHADER_FILES})
  target_link_libraries(${EXPNAME} ${ALL_LIBRARIES})

  source_group("Source Files" FILES ${SOURCE_FILES})
  source_group("Shader Files" FILES ${SHADER_FILES})
endfunction(BUILD_EXAMPLE)

file(GLOB SUBDIR_LIST RELATIVE ${CMAKE_CURRENT_LIST_DIR} "*")
foreach(SUBDIR ${SUBDIR_LIST})
  if (IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${SUBDIR}")
    BUILD_EXAMPLE(${SUBDIR})
  endif()
endforeach()
