cmake_minimum_required (VERSION 3.5.1)

project (IBEX_AMPL VERSION 1.0 LANGUAGES CXX)
set (IBEX_AMPL_DESCRIPTION "Plugin to interface ibex and AMPL")
set (IBEX_AMPL_URL "https://www.ibex-lib.org")
set (IBEX_MIN_VERSION_REQ 2.8.9.1)

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
# Configure the libraries for ASL and AMPL
################################################################################
add_subdirectory (3rd)



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
