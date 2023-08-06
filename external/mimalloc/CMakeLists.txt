add_library(mimalloc STATIC
	src/stats.c
	src/random.c
	src/os.c
	src/bitmap.c
	src/arena.c
	src/segment-cache.c
	src/segment.c
	src/page.c
	src/alloc.c
	src/alloc-aligned.c
	src/alloc-posix.c
	src/heap.c
	src/options.c
	src/init.c)
target_include_directories(mimalloc SYSTEM PUBLIC include)
set_target_properties(mimalloc PROPERTIES FOLDER external)
