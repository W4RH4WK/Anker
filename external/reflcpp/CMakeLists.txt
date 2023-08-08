add_library(reflcpp INTERFACE)
target_compile_definitions(reflcpp INTERFACE REFL_NO_STD_SUPPORT)
target_include_directories(reflcpp SYSTEM INTERFACE .)
set_target_properties(reflcpp PROPERTIES FOLDER external)
