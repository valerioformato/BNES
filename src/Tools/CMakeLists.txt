set(Tools_SRC CPUDebugger.cpp PPUDebugger.cpp)

add_library(NESTools STATIC ${Tools_SRC})
target_link_libraries(NESTools NESHW SDLBind)
