#pragma once

#include <string>
#include <json/json.h>

struct json_object {
	Json::Value ptr;
};

namespace utils
{
	std::string get_current_directory();
	std::string get_user_directory();
	std::string random_string(std::string::size_type length, const std::string& chars);
	std::string to_lower(const std::string& string);
	json_object parse_json(const std::string& content);

	namespace timestamp
	{
		long long get();
		std::string fmt(long long timestamp);
	}
}