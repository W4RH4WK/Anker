add_library(fmt STATIC
	include/fmt/args.h
	include/fmt/chrono.h
	include/fmt/color.h
	include/fmt/compile.h
	include/fmt/core.h
	include/fmt/format-inl.h
	include/fmt/format.h
	include/fmt/os.h
	include/fmt/ostream.h
	include/fmt/printf.h
	include/fmt/ranges.h
	include/fmt/std.h
	include/fmt/xchar.h
	src/format.cc
	src/os.cc)
target_include_directories(fmt SYSTEM PUBLIC include)
set_target_properties(fmt PROPERTIES FOLDER external)
