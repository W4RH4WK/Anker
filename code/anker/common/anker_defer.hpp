#pragma once

#include "anker_macros.hpp"

// With the defer utility you can define a function that will be executed
// automatically at the end of the current scope.
#define ANKER_DEFER const auto ANKER_TEMPORARY(ankerDeferer) = ::Anker::Internal::DeferHolderTag{} + [&]()

namespace Anker::Internal {

enum DeferHolderTag {};

template <typename T>
class DeferHolder {
  public:
	explicit DeferHolder(T&& function) : m_function(std::move(function)) {}
	DeferHolder(const DeferHolder&) = delete;
	DeferHolder& operator=(const DeferHolder&) = delete;
	DeferHolder(const DeferHolder&&) noexcept = delete;
	DeferHolder& operator=(const DeferHolder&&) noexcept = delete;
	~DeferHolder() noexcept { m_function(); }

  private:
	T m_function;
};

template <typename T>
inline DeferHolder<T> operator+(DeferHolderTag, T&& function)
{
	return DeferHolder<T>(std::forward<T>(function));
}

} // namespace Anker::Internal
