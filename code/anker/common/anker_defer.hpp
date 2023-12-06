#pragma once

#include <anker/common/anker_macros.hpp>

// With the defer utility you can define code that will be executed
// automatically at the end of the current scope.
#define ANKER_DEFER(code) const auto ANKER_TEMPORARY(ankerDeferer) = ::Anker::Internal::DeferHolder([&]() { code; });

namespace Anker::Internal {

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

} // namespace Anker::Internal
