cmake_minimum_required(VERSION 3.16)

project(
  BNES
  VERSION 0.0.1
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(vendor/SDL)
add_subdirectory(vendor/magic_enum)

set(SPDLOG_USE_STD_FORMAT
    OFF
    CACHE BOOL "use std::format for formatting" FORCE
)
set(SPDLOG_FMT_EXTERNAL
    OFF
    CACHE BOOL "use external fmt library" FORCE
)
add_subdirectory(vendor/spdlog)

message(STATUS "${SPDLOG_USE_STD_FORMAT}")
message(STATUS "${SPDLOG_FMT_EXTERNAL}")

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

# Compiler-specific warning flags
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

add_subdirectory(src)

if(ENABLE_BNES_TESTS)
  add_subdirectory(vendor/Catch2)

  include(CTest)
  include(Catch)

  add_subdirectory(testsuite)
endif()
