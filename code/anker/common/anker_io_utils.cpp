#include <anker/common/anker_io_utils.hpp>

#include <base64.h>

namespace Anker {

std::string encodeBase64(std::string_view sv)
{
	return base64_encode(sv);
}

std::string decodeBase64(std::string_view sv)
{
	return base64_decode(sv);
}

} // namespace Anker
