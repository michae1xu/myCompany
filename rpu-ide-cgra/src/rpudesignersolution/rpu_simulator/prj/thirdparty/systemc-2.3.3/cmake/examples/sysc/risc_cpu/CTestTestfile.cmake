# CMake generated Testfile for 
# Source directory: C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu
# Build directory: C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(examples/sysc/risc_cpu/risc_cpu "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu/Debug/risc_cpu.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/risc_cpu/risc_cpu PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;86;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(examples/sysc/risc_cpu/risc_cpu "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu/Release/risc_cpu.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/risc_cpu/risc_cpu PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;86;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(examples/sysc/risc_cpu/risc_cpu "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu/MinSizeRel/risc_cpu.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/risc_cpu/risc_cpu PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;86;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(examples/sysc/risc_cpu/risc_cpu "D:/program/cmake/bin/cmake.exe" "-DTEST_EXE=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu/RelWithDebInfo/risc_cpu.exe" "-DTEST_DIR=C:/Users/jun/Downloads/systemc-2.3.3/cmake/examples/sysc/risc_cpu" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=C:/Program Files/Git/usr/bin/diff.exe" "-DDIFF_OPTIONS=-u" "-P" "C:/Users/jun/Downloads/systemc-2.3.3/cmake/run_test.cmake")
  set_tests_properties(examples/sysc/risc_cpu/risc_cpu PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "C:/Users/jun/Downloads/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;86;configure_and_add_test;C:/Users/jun/Downloads/systemc-2.3.3/examples/sysc/risc_cpu/CMakeLists.txt;0;")
else()
  add_test(examples/sysc/risc_cpu/risc_cpu NOT_AVAILABLE)
endif()
