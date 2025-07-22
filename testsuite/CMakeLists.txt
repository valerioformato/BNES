add_executable(run_tests run_tests.cpp CPU/cpu_tests.cpp CPU/cpu_tests_decode.cpp CPU/cpu_tests_execute.cpp)

target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/testsuite ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(run_tests Catch2::Catch2WithMain NESHW)

catch_discover_tests(run_tests)
