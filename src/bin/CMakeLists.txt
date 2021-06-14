
list (APPEND SRC2 ${CMAKE_CURRENT_SOURCE_DIR}/ibexopt.cpp 
				  ${CMAKE_CURRENT_SOURCE_DIR}/parse_args.h
                  )

# Create the target for ibexopt
add_executable (ibexopt ${SRC2})
target_link_libraries (ibexopt PUBLIC ibex-ampl)
set_target_properties (ibexopt PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                      "${CMAKE_BINARY_DIR}/bin")

install (TARGETS ibexopt DESTINATION ${CMAKE_INSTALL_BINDIR}
                           COMPONENT binaries)
