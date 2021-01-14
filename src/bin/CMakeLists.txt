add_executable (ibexopt-ampl ibexopt-ampl.cpp parse_args.h)
target_link_libraries (ibexopt-ampl ibex asl)
set_target_properties (ibexopt-ampl PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                      "${CMAKE_BINARY_DIR}/bin")

install (TARGETS ibexopt-ampl DESTINATION ${CMAKE_INSTALL_BINDIR}
                           COMPONENT binaries)
