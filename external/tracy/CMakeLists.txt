add_library(tracy STATIC
	public/TracyClient.cpp)
target_compile_definitions(tracy PUBLIC
	TRACY_ENABLE
	TRACY_ONLY_LOCALHOST)
target_include_directories(tracy SYSTEM PUBLIC public)
set_target_properties(tracy PROPERTIES FOLDER external)
