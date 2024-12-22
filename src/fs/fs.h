#pragma once

#include <string>
#include <vector>
#include <filesystem>

#define MAX_FILES    1024
#define MAX_SIMILARS 256

typedef enum {
	fmFiles,
	fmRecursive,
} find_mode;

enum fs_exist_stat {
	existNone,
	existObject,
	existDirectory,
};

struct fs_obj {
	fs_exist_stat _status;
	bool as(const fs_exist_stat status) {
		return _status == status;
	}
};

namespace fs
{
	fs_obj exists(const std::string& path);
	long get_file_size(const std::string& filename);
	bool make_directory(const std::string& dirname, int attributes = 0);
	bool make_file(const std::string& filename, const std::string& content = "", int flags = std::ios::out | std::ios::trunc);
	std::string get_file_content(const std::string& filename);
	void get_directory_files(const std::string& dirname, char** files, int* num, find_mode mode);
	void find_similar_files(const std::string& keyword, const std::string& dirname, char* similars[MAX_SIMILARS], int* num);
	bool find_files_in_directories(const std::string& dirname, const std::string& keyword);
	void copy(const std::string& source, const std::string& destination, const std::filesystem::copy_options options);
}