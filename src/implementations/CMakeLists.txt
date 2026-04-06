
# Color

add_library(Color_hueToRgb
    color_hueToRgb.c
)
add_library(Color_violet
    color_violet.c
)
add_library(Color_yellow
    color_yellow.c
)

add_library(Color INTERFACE)

target_link_libraries(Color
    INTERFACE Color_hueToRgb
    INTERFACE Color_violet
    INTERFACE Color_yellow
)

# Statemachine

add_library(Statemachine_Basics
    statemachine_Basics.c
)
add_library(Statemachine_Reset
    statemachine_Reset.c
)

add_library(Statemachine INTERFACE)

target_link_libraries(Statemachine
    INTERFACE Statemachine_Basics
    INTERFACE Statemachine_Reset
)
