#pragma once

#include "anker_type_utils.hpp"

namespace Anker {

// Specialize this template with To template parameter and an explicit argument
// to implement the `as` conversion operator.
template <typename To>
To as(const auto&)
{
	static_assert(AlwaysFalse<To>, "Conversion not implemented");
	return {};
}

} // namespace Anker
