# FindFilib
# -----------
#
# The module defines the following variables:
#
#     ASL_FOUND
#     ASL_INCLUDE_DIRS
#     ASL_LIBRARIES
#     ASL_VERSION
#     
#
# and the following imported target (if it does not already exist):
#
#  ASL - The AMPL Solver Library
#
#
# Requires CMake >= 3.0

include(CheckCXXCompilerFlag)

set (ASL_DIR "${ASL_DIR}" CACHE PATH "Directory to search for ASL")

# Look for the library
find_library (ASL_LIBRARY NAMES asl HINTS "${ASL_DIR}" PATH_SUFFIXES lib)

# Might want to look close to the library first for the includes.
get_filename_component (_libdir "${ASL_LIBRARY}" PATH)

# Look for the include directory
find_path (ASL_INC_DIR NAMES asl/asl.h
                         HINTS "${_libdir}/.." "${ASL_DIR}"
                         PATH_SUFFIXES include)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (asl DEFAULT_MSG ASL_LIBRARY ASL_INC_DIR)

# TODO mais je comprend pas la syntaxe
#find_package_handle_standard_args (dl DEFAULT_MSG dl)

if (ASL_FOUND)
  set (ASL_LIBRARIES ${ASL_LIBRARY})
  set (ASL_INCLUDE_DIRS "${ASL_INC_DIR}")
  #Todo
  set (ASL_VERSION "????")
  mark_as_advanced (ASL_DIR)
  if (NOT TARGET asl::asl)
    # For now we make the target global, because this file is included from a
    # CMakeLists.txt file in a subdirectory. With CMake >= 3.11, we could make
    # it global afterwards with
    # set_target_properties(asl::asl PROPERTIES IMPORTED_GLOBAL TRUE)
    add_library (asl::asl INTERFACE IMPORTED GLOBAL)
    set_target_properties (asl::asl PROPERTIES
                        INTERFACE_INCLUDE_DIRECTORIES "${ASL_INCLUDE_DIRS}"
                        INTERFACE_LINK_LIBRARIES "${ASL_LIBRARIES}")

#    # Checking for -frounding-math and -ffloat-store
#    include(CheckCXXCompilerFlag)
#    check_cxx_compiler_flag ("-frounding-math" COMPILER_SUPPORTS_FROUNDING_MATH)
#    if (COMPILER_SUPPORTS_FROUNDING_MATH)
#      list (APPEND FILIB_COMPILE_OPTIONS "-frounding-math")
#    endif ()
#    if (NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
#      check_cxx_compiler_flag ("-ffloat-store" COMPILER_SUPPORTS_FFLOAT_STORE)
#      if (COMPILER_SUPPORTS_FROUNDING_MATH)
#        list (APPEND FILIB_COMPILE_OPTIONS "-ffloat-store")
#      endif ()
#    endif ()
    set_target_properties (asl::asl PROPERTIES
                          INTERFACE_COMPILE_OPTIONS "${ASL_COMPILE_OPTIONS}")
  endif()
endif()

mark_as_advanced(ASL_INC_DIR ASL_LIBRARY)
