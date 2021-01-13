# If the tests need the CppUnit library, one can use the following command to
# look for it
find_package (CppUnit)
# The script FindCppUnit.cmake is installed by Ibex and the CMAKE_MODULE_PATH
# is set accordingly by ibex_init_common.
# Once the command is executed, the variable CPPUNIT_FOUND can be use to test if
# the library was found or not, and the imported target CppUnit::CppUnit can be
# use in target_link_libraries to link against it.

# Only add the test targets if cppunit was found
if (CPPUNIT_FOUND)
  # Compile common stuff for the tests
  add_library (test_common utest.cpp utest.h utils.cpp utils.h)
  target_link_libraries (test_common PUBLIC ibex-ampl)

  set (TESTS_LIST TestAmpl)

  foreach (test ${TESTS_LIST})
    # /!\ The test and the target building the executable have the same name
    add_executable (${test} ${test}.cpp ${test}.h)
    target_link_libraries (${test} test_common CppUnit::CppUnit)
    add_dependencies (check ${test})
    add_test (${test} ${test})
  endforeach ()
else ()
  message (STATUS "Will not run tests, required cppunit library was not found")
  set (MSG "No tests will be run as CMake failed to find the cppunit library \
during the configuration step. If you want to run the tests, you need to \
install cppunit. Then, you need to re-run the configuration step of CMake. You \
can use the CPPUNIT_DIR variable to indicate to CMake were to find it")
  add_custom_target (notest COMMAND ${CMAKE_COMMAND} -E echo ${MSG})
  add_dependencies (check notest)
endif ()
