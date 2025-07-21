cmake_minimum_required(VERSION 3.16)

project(
  BNES
  VERSION 0.0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/magic_enum)

# -------------------------------------------------------------------
# -------------------------- Begin project --------------------------
# -------------------------------------------------------------------

option(ASAN "Enable Address sanitizer" OFF)
option(TSAN "Enable Thread sanitizer" OFF)
option(ENABLE_BNES_TESTS "Enable unit test compilation" OFF)

if(ASAN)
  message(STATUS "Enabling Address sanitizer")
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)

  if(NOT APPLE)
    add_compile_options(-fsanitize=leak)
    add_link_options(-fsanitize=leak)
  endif()
endif()

if(TSAN)
  message(STATUS "Enabling Thread sanitizer")
  add_compile_options(-fsanitize=thread)
  add_link_options(-fsanitize=thread)
endif()

add_compile_options(-Wall -Wextra)

add_subdirectory(src)

if(ENABLE_BNES_TESTS)
  add_subdirectory(vendor/Catch2)

  include(CTest)
  include(Catch)

  add_subdirectory(testsuite)
endif()
