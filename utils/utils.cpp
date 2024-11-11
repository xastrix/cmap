#include "utils.h"

#include <ctime>
#include <chrono>
#include <filesystem>
#include <windows.h>

std::string utils::get_current_directory()
{
	TCHAR dir[MAX_PATH];
	GetModuleFileNameA(0, dir, MAX_PATH);

	return std::filesystem::path{ dir }.parent_path().string();
}

std::string utils::get_user_directory()
{
	return std::string{ getenv("USERPROFILE") } + "\\";
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

long long utils::timestamp::get()
{
	auto now = std::chrono::system_clock::now();
	auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	return milliseconds_since_epoch;
}

std::string utils::timestamp::fmt(long long timestamp)
{
	long long diff    = get() - timestamp;
	long long seconds = diff / 1000;
	long long hours   = seconds / 3600;
	long long minutes = (seconds % 3600) / 60;

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