cmake_minimum_required (VERSION 3.0.2)

project (IBEX_AMPL_EXAMPLES LANGUAGES CXX)

include (FindPkgConfig)
pkg_search_module (IBEX REQUIRED ibex ibex-ampl)
message (STATUS "Found Ibex version ${IBEX_VERSION}")

add_compile_options(-O3 -DNDEBUG)

if (WIN32)
  add_definitions(-U__STRICT_ANSI__)
endif ()

#
set (AMPL_EXAMPLES test_ampl)

foreach (ex_name ${AMPL_EXAMPLES})
  add_executable (${ex_name} ${ex_name}.cpp)
  target_compile_options (${ex_name} PUBLIC ${IBEX_CFLAGS})
  target_include_directories (${ex_name} PUBLIC ${IBEX_INCLUDE_DIRS})
  target_link_libraries (${ex_name} PUBLIC ${IBEX_LDFLAGS})
endforeach ()
