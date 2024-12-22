#include "utils.h"

#include "../fs/fs.h"

#include <ctime>
#include <windows.h>
#include <cctype>

std::string utils::get_current_directory()
{
	TCHAR dir[MAX_PATH];
	GetModuleFileNameA(0, dir, MAX_PATH);

	return std::filesystem::path{ dir }.parent_path().string();
}

std::string utils::get_user_directory()
{
	return getenv("USERPROFILE");
}

std::string utils::random_string(std::string::size_type length, const std::string& chars)
{
	std::string str{};
	str.reserve(length);

	srand(time(0));

	auto dictonary_length = chars.length() - 1;
	for (int i = 0; i < length; i++) {
		unsigned int random_index = rand() % dictonary_length;
		str += chars.at(random_index);
	}

	return str;
}

std::string utils::to_lower(const std::string& string)
{
	std::string ret = string;

	for (char &c : ret) {
		c = std::tolower(static_cast<unsigned char>(c));
	}

	return ret;
}

json_object utils::parse_json(const std::string& filename)
{
	json_object ret;

	std::istringstream ss{ fs::get_file_content(filename) };
	ret.parse = Json::parseFromStream(ret.reader_builder, ss, &ret.ptr, &ret.log);

	return ret;
}

int64_t utils::timestamp::get()
{
	auto now = std::chrono::system_clock::now();
	auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	return milliseconds_since_epoch;
}

std::string utils::timestamp::fmt(int64_t timestamp)
{
	int64_t diff    = get() - timestamp;
	int64_t seconds = diff / 1000;
	int64_t hours   = seconds / 3600;
	int64_t minutes = (seconds % 3600) / 60;

	std::string result;

	if (hours > 0) {
		result += std::to_string(hours) + "h ";
	}

	if (minutes > 0 || hours > 0) {
		result += std::to_string(minutes) + "min ago";
	}

	if (result.empty()) {
		result = "just now";
	}

	return result;
}