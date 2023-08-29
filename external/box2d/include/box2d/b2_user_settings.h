#include <stdarg.h>
#include <stdint.h>

// Tunable Constants

/// You can use this to change the length scale used by your game.
/// For example for inches you could use 39.4.
#define b2_lengthUnitsPerMeter 1.0f

/// The maximum number of vertices on a convex polygon. You cannot increase
/// this too much because b2BlockAllocator has a maximum object size.
#define b2_maxPolygonVertices	8

// User data

/// You can define this to inject whatever data you want in b2Body
struct B2_API b2BodyUserData
{
	b2BodyUserData()
	{
		pointer = 0;
	}

	/// For legacy compatibility
	uintptr_t pointer;
};

/// You can define this to inject whatever data you want in b2Fixture
struct B2_API b2FixtureUserData
{
	b2FixtureUserData()
	{
		pointer = 0;
	}

	/// For legacy compatibility
	uintptr_t pointer;
};

/// You can define this to inject whatever data you want in b2Joint
struct B2_API b2JointUserData
{
	b2JointUserData()
	{
		pointer = 0;
	}

	/// For legacy compatibility
	uintptr_t pointer;
};

namespace Anker {
void* b2Alloc(int32 size);
void b2Free(void* mem);
void b2Log(const char* string, va_list args);
} // namespace Anker

// Memory Allocation

/// Implement this function to use your own memory allocator.
inline void* b2Alloc(int32 size)
{
	return Anker::b2Alloc(size);
}

/// If you implement b2Alloc, you should also implement this function.
inline void b2Free(void* mem)
{
	Anker::b2Free(mem);
}

/// Implement this to use your own logging.
inline void b2Log(const char* string, ...)
{
	va_list args;
	va_start(args, string);
	Anker::b2Log(string, args);
	va_end(args);
}
