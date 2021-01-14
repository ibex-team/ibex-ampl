################################################################################
# First example: compiling a library that depends on libibex
################################################################################
# source files of libibex-ampl
list (APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/system/ibex_AmplInterface.cpp
                 ${CMAKE_CURRENT_SOURCE_DIR}/system/ibex_AmplInterface.h
                 )

# Create the target for libibex-ampl
add_library (ibex-ampl ${SRC})
target_include_directories (ibex-ampl PUBLIC
                            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/system>"
                			"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/ibex-ampl>")
target_link_libraries (ibex-ampl PUBLIC Ibex::ibex asl::asl)

# installation of libibex-ampl files
ibex_list_filter_header (HDR ${SRC}) # Ibex should have installed this function
install (TARGETS ibex-ampl EXPORT AmplExport
                               DESTINATION ${CMAKE_INSTALL_LIBDIR})
install (FILES ${HDR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ibex-ampl)

################################################################################
# More examples in the bin subdirectory
################################################################################
add_subdirectory (bin)
