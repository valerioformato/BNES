set(test_SRC run_unit_tests.cpp)

add_subdirectory(HW)

# Configure the localization header
configure_file(localization.h.in ${CMAKE_CURRENT_BINARY_DIR}/localization.h @ONLY)

add_executable(run_unit_tests ${test_SRC})

# Set the include folder of the localization header
target_include_directories(run_unit_tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Link against your project libraries and Catch2
target_include_directories(run_unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(run_unit_tests PRIVATE NESHW SDLBind Catch2::Catch2WithMain)

include(CTest)
include(Catch)

catch_discover_tests(run_unit_tests)
