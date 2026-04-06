
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
