add_executable(run_tests run_tests.cpp CPU/cpu_tests.cpp CPU/cpu_tests_decode.cpp CPU/cpu_tests_execute.cpp)

# Link against your project libraries and Catch2
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(run_tests PRIVATE NESHW SDLBind Catch2::Catch2WithMain)

add_test(NAME run_tests COMMAND ${CMAKE_CURRENT_BINARY_DIR}/run_tests)
