add_subdirectory(CPU)
add_subdirectory(PPU)
add_subdirectory(Rom)
add_subdirectory(Bus)

set(test_SRC
        ${test_SRC}
        PARENT_SCOPE
)
