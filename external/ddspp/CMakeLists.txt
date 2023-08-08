add_library(ddspp INTERFACE)
target_include_directories(ddspp SYSTEM INTERFACE .)
set_target_properties(ddspp PROPERTIES FOLDER external)
