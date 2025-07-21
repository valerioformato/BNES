add_library(NESHW STATIC CPU.cpp)
target_link_libraries(NESHW PUBLIC magic_enum::magic_enum)
