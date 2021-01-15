set (ASL_NAME "asl")
set (ASL_NAME ${ASL_NAME} PARENT_SCOPE)

# Look for ASL
include (FindASL.cmake)
include (ibex-install-3rd)

if (ASL_FOUND)
  set (ASL_TARGET "asl::asl" PARENT_SCOPE)
  # Install FindASL.cmake file
  install (FILES FindASL.cmake
            DESTINATION ${CMAKE_INSTALL_CONFIGCMAKE}/cmake.utils
            COMPONENT devtools)
  # Write CMake file containing path hint and install it
  generate_path_hints_file (ASL INSTALL)
else ()
  message (STATUS "Will install and use library ASL from 3rd/ subdirectory")

  ###
  set (ASL_VERSION "20201107")
  set (ASL_basename "asl")
  set (ASL_archive_name "asl.tar.gz")
  set (ASL_archive "${CMAKE_CURRENT_SOURCE_DIR}/${ASL_archive_name}")
  set (wdir "${CMAKE_CURRENT_BINARY_DIR}/${ASL_basename}")
  set (logdir "${CMAKE_CURRENT_BINARY_DIR}/log")

  file (MAKE_DIRECTORY "${logdir}")

  ### Extract
  execute_process_check (COMMAND ${CMAKE_COMMAND} -E tar xzf ${ASL_archive}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                         MSG "extracting ${ASL_archive_name}"
                         LOGBASENAME "${logdir}/extract"
                         STATUS_PREFIX "${ASL_NAME}: ")


  ### Configure
  message (STATUS "${ASL_NAME}: configure cmake")
  # Original values will be restored at the end of this file
  set (CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR_3RD})
  set (CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR_3RD})
  add_subdirectory (${wdir} ${wdir}-build)
  message (STATUS "${ASL_NAME}: configure - done")

  ###
  set (ASL_TARGET "asl::asl" PARENT_SCOPE)
  set (ASL_VERSION ${ASL_VERSION} PARENT_SCOPE)
endif ()


