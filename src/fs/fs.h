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

enum dir_attrib {
	da_def,
	da_hidden = 2,
};

enum fs_exist_stat {
	existNone,
	existObject,
	existDirectory,
};

enum fs_make_stat {
	fsm_perm_denied,
	fsm_ok,
};

struct fs_obj {
	fs_exist_stat _status;
	bool as(const fs_exist_stat status) {
		return _status == status;
	}
};

namespace fs
{
	/*
	 * Checking for the existence of an object (file/directory) on the specified path
	 * @param path to the object
	 * @return fs_obj structure with as() method
	*/
	fs_obj exists(const std::string& path);

	/*
	 * Get file size by specified file name
	 * @param filename
	 * @return size of file in long
	*/
	long get_file_size(const std::string& filename);

	/*
	 * Creating a directory by a specified name
	 * @param dirname
	 * @param dir attributes
	 * @return fs_make_stat
	*/
	fs_make_stat make_directory(const std::string& dirname, dir_attrib attributes = da_def);

	/*
	 * Creating a file by a specified name
	 * @param filename
	 * @param file content
	 * @param file flags
	 * @return fs_make_stat
	*/
	fs_make_stat make_file(const std::string& filename, const std::string& content = "", int flags = std::ios::out | std::ios::trunc);
	
	/*
	 * Deleting files and directories specified in the provided array
	 * @param array
	*/
	void delete_objects(const std::vector<std::string>& objects);

	/*
	 * Retrieving file content by specified name
	 * @param filename
	 * @return file content
	*/
	std::string get_file_content(const std::string& filename);

	/*
	 * Getting the list of files in the specified directory
	 * @param dirname
	 * @param allocated array for the list
	 * @param number of files found in the directory
	 * @param file search type (fmFiles - search for files only, fmRecursive - search for files and folders)
	*/
	void get_directory_files(const std::string& dirname, char** files, int* num, find_mode mode);

	/*
	 * Search for similar files in a specified directory by keyword
	 * @param keyword
	 * @param dirname
	 * @param allocated array under the list of similar files
	 * @param number of similar files found in the directory
	*/
	void find_similar_files(const std::string& keyword, const std::string& dirname, char* similars[MAX_SIMILARS], int* num);

	/*
	 * Search for files in the directory by keyword
	 * @param dirname
	 * @param keyword
	 * @return true/false
	*/
	bool find_files_in_directories(const std::string& dirname, const std::string& keyword);

	/*
	 * Copying an object from one directory to another directory
	 * @param source
	 * @param destination
	 * @param copy type
	*/
	void copy(const std::string& source, const std::string& destination, const std::filesystem::copy_options options);
}