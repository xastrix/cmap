#pragma once

#include <string>
#include <json/json.h>

struct json_object {
	Json::Value ptr;
	Json::CharReaderBuilder reader_builder;
	Json::String log;
	bool parse;
};

namespace utils
{
	std::string get_current_directory();
	std::string get_user_directory();
	std::string random_string(std::string::size_type length, const std::string& chars);
	std::string to_lower(const std::string& string);
	json_object parse_json(const std::string& filename);

	namespace timestamp
	{
		int64_t get();
		std::string fmt(int64_t timestamp);
	}
}