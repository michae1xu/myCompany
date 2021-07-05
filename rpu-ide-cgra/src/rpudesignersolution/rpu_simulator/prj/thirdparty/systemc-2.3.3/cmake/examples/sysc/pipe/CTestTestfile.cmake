# CMake generated Testfile for 
# Source directory: C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe
# Build directory: C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(examples/sysc/pipe/pipe "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe/Debug/pipe.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/pipe/pipe PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;54;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(examples/sysc/pipe/pipe "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe/Release/pipe.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/pipe/pipe PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;54;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(examples/sysc/pipe/pipe "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe/MinSizeRel/pipe.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/pipe/pipe PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;54;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(examples/sysc/pipe/pipe "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe/RelWithDebInfo/pipe.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/pipe" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/pipe/pipe PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;54;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/pipe/CMakeLists.txt;0;")
else()
  add_test(examples/sysc/pipe/pipe NOT_AVAILABLE)
endif()
