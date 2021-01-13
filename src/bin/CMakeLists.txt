add_executable (ibex-ampl ibex-ampl.cpp parse_args.h)
target_link_libraries (ibex-ampl ibex asl)
set_target_properties (ibex-ampl PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                      "${CMAKE_BINARY_DIR}/bin")

install (TARGETS ibex-ampl DESTINATION ${CMAKE_INSTALL_BINDIR}
                           COMPONENT binaries)
