add_library(NESHW STATIC CPU.cpp OpCodes.h
)
target_link_libraries(NESHW PUBLIC magic_enum::magic_enum spdlog::spdlog)
