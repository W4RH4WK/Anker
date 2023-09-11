add_library(cppbase64 STATIC base64.cpp)
target_include_directories(cppbase64 SYSTEM PUBLIC .)
set_target_properties(cppbase64 PROPERTIES
	CXX_STANDARD 20
	FOLDER external)
