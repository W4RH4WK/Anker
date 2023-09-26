#include "anker_math.hpp"

#include "anker_string_utils.hpp"

namespace Anker {

bool colorFromHtml(Vec4& color, std::string_view input)
{
	using namespace Ascii;

	// skip optional `#`
	if (input.size() > 0 && input[0] == '#') {
		input = input.substr(1);
	}

	if (input.size() != 6) {
		return false;
	}

	std::array digits = {
	    toLowerChar(input[0]), toLowerChar(input[1]), toLowerChar(input[2]),
	    toLowerChar(input[3]), toLowerChar(input[4]), toLowerChar(input[5]),
	};

	if (!std::ranges::all_of(digits, [](char c) { return '0' <= c && c <= 'f'; })) {
		return false;
	}

	std::ranges::transform(digits, digits.begin(), [](char c) { return c - '0'; });

	color.x = (16.0f * digits[0] + digits[1]) / 255.0f;
	color.y = (16.0f * digits[2] + digits[3]) / 255.0f;
	color.z = (16.0f * digits[4] + digits[5]) / 255.0f;
	color.w = 1.0f;
	return true;
}

} // namespace Anker
