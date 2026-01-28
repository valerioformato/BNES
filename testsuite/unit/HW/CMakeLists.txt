add_subdirectory(CPU)
add_subdirectory(PPU)
add_subdirectory(Rom)
add_subdirectory(Bus)

set(test_SRC
    ${test_SRC} HW/nmi_integration_tests.cpp
    PARENT_SCOPE
)
