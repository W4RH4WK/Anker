add_library(glm INTERFACE)
target_compile_definitions(glm INTERFACE
	GLM_FORCE_DEPTH_ZERO_TO_ONE
	GLM_FORCE_XYZW_ONLY)
target_include_directories(glm SYSTEM INTERFACE .)
set_target_properties(glm PROPERTIES FOLDER external)
