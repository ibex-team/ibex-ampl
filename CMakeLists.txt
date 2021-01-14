cmake_minimum_required (VERSION 3.5.1)

project (IBEX_AMPL VERSION 1.0 LANGUAGES CXX)
set (IBEX_AMPL_DESCRIPTION "SPlugin to interface ibex and AMPL")
set (IBEX_AMPL_URL "https://www.ibex-lib.org")
set (IBEX_MIN_VERSION_REQ 2.8)

find_package (IBEX ${IBEX_MIN_VERSION_REQ} REQUIRED)
message (STATUS "Found Ibex version ${IBEX_VERSION}")
ibex_init_common () # Ibex should have installed this function
set (CMAKE_INSTALL_CONFIGCMAKE "share/ibex-ampl/cmake")

# Interval and linear programming names and versions are available
message (STATUS "Ibex was compiled with interval library "
                "${IBEX_INTERVAL_LIB_NAME} ${IBEX_INTERVAL_LIB_VERSION}")

if (IBEX_LP_LIB_NAME) # Was Ibex compiled with a linear programming library ?
  message (STATUS "Ibex was compiled with Linear Programming library "
                  "${IBEX_LP_LIB_NAME} ${IBEX_LP_LIB_VERSION}")
else ()
  message (STATUS "Ibex was compiled without any Linear Programming library")
endif ()
################################################################################

################################################################################
# Add cmake.utils to the list of CMAKE_MODULE_PATH
################################################################################
set (MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake.utils")
list (APPEND CMAKE_MODULE_PATH "${MODULE_DIR}")
################################################################################
# Run common init function and install the module so plugins can use it
################################################################################
#message (STATUS "Configuring Ibex ${IBEX_VERSION}")
#include (ibexAmpl-config-utils)
#ibex_init_common ()

################################################################################
# Options
################################################################################
set (ASL_DIR "" CACHE PATH "Path to the ASL lib and include directories")


################################################################################
# May be needed to install missing dependencies
################################################################################
#include (ExternalProject)
include (IbexAmplUtils)

# Looking for asl/asl.h, result is written in ASL_INCDIR
find_header_custom (ASL "asl/asl.h" PATHS "${ASL_DIR}")
# Looking for asl library, result is written in ASL_LIB
find_library_custom (ASL "asl" PATHS "${ASL_DIR}")



if (ASL_INCDIR AND ASL_LIB)
  create_target_import_and_export (asl ${ASL_LIB} ASL_EXPORTFILE
                                        #${filib_compile_option}
                                        NAMESPACE Ibex::
                                        TARGET_NAME asl
                                        INCLUDE_DIRECTORIES ${ASL_INCDIR})
else ()
  message (STATUS "Will install and use library ASL from 3rd/ subdirectory")

  set (asl_basename "asl")
  set (asl_archive_name "${asl_basename}.zip")
  set (asl_archive "${CMAKE_CURRENT_SOURCE_DIR}/3rd/${asl_archive_name}")
  set (wdir "${CMAKE_CURRENT_BINARY_DIR}/${asl_basename}")
  set (logdir "${CMAKE_CURRENT_BINARY_DIR}/log")
  set (_prefix "ASL: ")

  file (MAKE_DIRECTORY "${logdir}")

  # extract
  execute_process_check (COMMAND ${CMAKE_COMMAND} -E tar xzf ${asl_archive}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                         MSG "extracting ${asl_archive_name}"
                         LOGBASENAME "${logdir}/extract"
                         STATUS_PREFIX "${_prefix}")

  # TODO apply patch
  #set (FILIB_PATCH "${CMAKE_CURRENT_SOURCE_DIR}/3rd/filibsrc-3.0.2.2.all.all.patch")

  # copy files
  foreach (file "CMakeLists.txt" "config.h.in" "rounding_control/rounding_control_config.hpp.in")
    set (infile ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${filib_basename}/${file})
    set (outfile ${wdir}/${file})
    string (REPLACE "/" "_" logbase "${file}")
    execute_process_check (COMMAND ${CMAKE_COMMAND} -E copy ${infile} ${outfile}
                           MSG "copying ${file}"
                           LOGBASENAME "${logdir}/copy-${logbase}"
                           STATUS_PREFIX "${_prefix}")
  endforeach ()

  # configure
  message (STATUS "${_prefix}configure")
  # Original values will be restored at the end of this file
  set (CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR_3RD})
  set (CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR_3RD})
  add_subdirectory (${wdir} ${wdir}-build)
  message (STATUS "${_prefix}configure - done")

  add_library (Ibex::asl ALIAS asl)
endif ()

list (APPEND EXPORTFILES "${ASL_EXPORTFILE}")
set (EXPORTFILES "${EXPORTFILES}" PARENT_SCOPE)


################################################################################
# Generate pkg-config file and install it (can be removed if no library is
# installed)
################################################################################
configure_file ("ibex-ampl.pc.in" "ibex-ampl.pc" @ONLY)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/ibex-ampl.pc
          DESTINATION ${CMAKE_INSTALL_PKGCONFIG})

################################################################################
# Compile sources
################################################################################
add_subdirectory (src)

################################################################################
# Generate CMake config file (can be removed if no library is installed)
################################################################################
set (IBEX_AMPL_TARGETS_CONFIG_FILE "ibex_ampl-targets.cmake")
install (EXPORT AmplExport FILE ${IBEX_AMPL_TARGETS_CONFIG_FILE}
                                NAMESPACE Ibex::
                                DESTINATION ${CMAKE_INSTALL_CONFIGCMAKE})
configure_file (ibex-ampl-config.cmake.in ibex_ampl-config.cmake @ONLY)
include (CMakePackageConfigHelpers)
set (VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/ibex_ampl-config-version.cmake")
write_basic_package_version_file (${VERSION_FILE} COMPATIBILITY AnyNewerVersion)
install (FILES ${VERSION_FILE}
               ${CMAKE_CURRENT_BINARY_DIR}/ibex_ampl-config.cmake
         DESTINATION ${CMAKE_INSTALL_CONFIGCMAKE})

################################################################################
# Tests
################################################################################
# The following command assumes that the tests are in a tests/ subdirectory. So
# the tests/ subdirectory must exist and must contain a CMakeLists.txt file.
# If this is not the case, comment the next line.
ibex_add_make_target_for_ctest (check)
