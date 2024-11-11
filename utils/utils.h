#pragma once

#include <string>

namespace utils
{
	std::string get_current_directory();
	std::string get_user_directory();
	std::string random_string(std::string::size_type length, const std::string& chars);

	namespace timestamp
	{
		long long get();
		std::string fmt(long long timestamp);
	}
}