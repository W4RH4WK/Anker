// This file overrides C++' memory allocation operators. We use this to forward
// allocation calls to mimalloc and enable profiling.

#if 1

#include <mimalloc.h>

// replaceable allocation functions

[[nodiscard]] void* operator new(size_t size)
{
	void* ptr = mi_new(size);
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

[[nodiscard]] void* operator new[](size_t size)
{
	void* ptr = mi_new(size);
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

[[nodiscard]] void* operator new(size_t size, std::align_val_t al)
{
	void* ptr = mi_new_aligned(size, static_cast<size_t>(al));
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

[[nodiscard]] void* operator new[](size_t size, std::align_val_t al)
{
	void* ptr = mi_new_aligned(size, static_cast<size_t>(al));
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

// replaceable non-throwing allocation functions

[[nodiscard]] void* operator new(size_t size, const std::nothrow_t&) noexcept
{
	void* ptr = mi_new_nothrow(size);
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

[[nodiscard]] void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
	void* ptr = mi_new_nothrow(size);
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

[[nodiscard]] void* operator new(size_t size, std::align_val_t al, const std::nothrow_t&) noexcept
{
	void* ptr = mi_new_aligned_nothrow(size, static_cast<size_t>(al));
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

[[nodiscard]] void* operator new[](size_t size, std::align_val_t al, const std::nothrow_t&) noexcept
{
	void* ptr = mi_new_aligned_nothrow(size, static_cast<size_t>(al));
	ANKER_PROFILE_ALLOC(ptr, size);
	return ptr;
}

// replaceable usual deallocation functions

void operator delete(void* ptr) noexcept
{
	ANKER_PROFILE_FREE(ptr);
	mi_free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	ANKER_PROFILE_FREE(ptr);
	mi_free(ptr);
}

void operator delete(void* ptr, std::align_val_t al) noexcept
{
	ANKER_PROFILE_FREE(ptr);
	mi_free_aligned(ptr, static_cast<size_t>(al));
}

void operator delete[](void* ptr, std::align_val_t al) noexcept
{
	ANKER_PROFILE_FREE(ptr);
	mi_free_aligned(ptr, static_cast<size_t>(al));
}

// replaceable placement deallocation functions

void operator delete(void* ptr, std::align_val_t al, const std::nothrow_t&) noexcept
{
	ANKER_PROFILE_FREE(ptr);
	mi_free_aligned(ptr, static_cast<size_t>(al));
}

void operator delete[](void* ptr, std::align_val_t al, const std::nothrow_t&) noexcept
{
	ANKER_PROFILE_FREE(ptr);
	mi_free_aligned(ptr, static_cast<size_t>(al));
}

#endif
