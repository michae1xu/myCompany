# CMake generated Testfile for 
# Source directory: C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt
# Build directory: C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(examples/tlm/lt/lt "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt/Debug/lt.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/results/expected.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/tlm/lt/lt PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;61;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(examples/tlm/lt/lt "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt/Release/lt.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/results/expected.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/tlm/lt/lt PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;61;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(examples/tlm/lt/lt "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt/MinSizeRel/lt.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/results/expected.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/tlm/lt/lt PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;61;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(examples/tlm/lt/lt "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt/RelWithDebInfo/lt.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/tlm/lt" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/results/expected.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/tlm/lt/lt PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;61;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/tlm/lt/CMakeLists.txt;0;")
else()
  add_test(examples/tlm/lt/lt NOT_AVAILABLE)
endif()
