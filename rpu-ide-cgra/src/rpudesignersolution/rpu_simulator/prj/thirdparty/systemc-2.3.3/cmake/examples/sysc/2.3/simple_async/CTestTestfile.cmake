# CMake generated Testfile for 
# Source directory: C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async
# Build directory: C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(examples/sysc/2.3/simple_async/simple_async "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async/Debug/simple_async.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/2.3/simple_async/simple_async PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;44;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(examples/sysc/2.3/simple_async/simple_async "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async/Release/simple_async.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/2.3/simple_async/simple_async PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;44;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(examples/sysc/2.3/simple_async/simple_async "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async/MinSizeRel/simple_async.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/2.3/simple_async/simple_async PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;44;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(examples/sysc/2.3/simple_async/simple_async "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async/RelWithDebInfo/simple_async.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/2.3/simple_async" "-DTEST_INPUT=" "-DTEST_GOLDEN=C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/2.3/simple_async/simple_async PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;44;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/2.3/simple_async/CMakeLists.txt;0;")
else()
  add_test(examples/sysc/2.3/simple_async/simple_async NOT_AVAILABLE)
endif()
