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
	/*
	 * Get the current directory
	 * @return directory
	*/
	std::string get_current_directory();

	/*
	 * Get the directory of the current user
	 * @return directory
	*/
	std::string get_user_directory();
	
	/*
	 * Generating a random string from the characters presented in the second argument of the function
	 * @param length
	 * @param charaters
	 * @return random string
	*/
	std::string random_string(std::string::size_type length, const std::string& chars);

	/*
	 * Function converting all letters in a string into small letters
	 * @param string
	 * @return modified string
	*/
	std::string to_lower(const std::string& string);

	/*
	 * Function of parsing JSON objects
	 * @param filename (With json data)
	 * @return json_object structure
	*/
	json_object parse_json(const std::string& filename);

	namespace timestamp
	{
		/*
		 * Get current timestamp
		 * @return timestamp
		*/
		int64_t get();

		/*
		 * Format timestamp in style (5 hours ago, 30 min ago, just now)
		 * @return format
		*/
		std::string fmt(int64_t timestamp);
	}
}