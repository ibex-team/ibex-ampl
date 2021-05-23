
list (APPEND SRC2 ${CMAKE_CURRENT_SOURCE_DIR}/ibexopt-ampl.cpp 
				  ${CMAKE_CURRENT_SOURCE_DIR}/parse_args.h
                  )

# Create the target for ibexopt-ampl
add_executable (ibexopt-ampl ${SRC2})
target_link_libraries (ibexopt-ampl PUBLIC ibex-ampl)
set_target_properties (ibexopt-ampl PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                      "${CMAKE_BINARY_DIR}/bin")

install (TARGETS ibexopt-ampl DESTINATION ${CMAKE_INSTALL_BINDIR}
                           COMPONENT binaries)
