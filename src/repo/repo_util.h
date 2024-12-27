#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../config.h"
#include "../cfg/cfg.h"

struct map_t {
	std::string hash, msg;
	cfg_t cfg;
	int64_t timestamp;
};

enum base_files_stat {
	alreadyExists,
	InitializeNew,
};

enum file_type {
	tempFiles,
	trackingFiles,
};

namespace repo::util
{
	/*
	 * Creating files with repository data
	 * @return base_files_stat
	*/
	base_files_stat setup_base_files();

	/*
	 * Retrieving files by member name
	 * @param type of files
	 * @return array of files
	*/
	std::vector<std::string> get_files(const file_type type);

	/*
	 * Retrieving untracked files
	 * @return array of untracked files
	*/
	std::vector<std::string> get_untracked_files();

	/*
	 * Retrieving modified files
	 * @return array of modified files
	*/
	std::vector<std::string> get_modified_files();

	/*
	 * Retrieving deleted files
	 * @return array of deleted files
	*/
	std::vector<std::string> get_deleted_files();

	/*
	* Copying all tracked files to the repository storage
	* @param hash
	* @return true/false
	*/
	bool copy_objects(const std::string& hash);

	/*
	 * Adding commit data to the repository map
	 * @param map_t structure with commit data
	 * @return true/false
	*/
	bool add_to_map(const map_t map);

	/*
	 * Adding objects to the tracked/untracked files
	 * @param type of file
	 * @param object/path
	*/
	void add_object_to_files(const file_type type, const std::string& obj);

	/*
	 * Deleting objects to the tracked/untracked files
	 * @param type of file
	 * @param object/path
	*/
	void remove_object_from_files(const file_type type, const std::string& obj);

	/*
	 * Check if a file is in the list of ignored files
	 * @param filename
	 * @return true/false
	*/
	bool is_ignore_file_detected(const std::string& filename);

	/*
	 * Getting data from the repository map about all commits
	 * @return array of map_t data
	*/
	std::vector<map_t> get_map_list();

	/*
	 * Getting data on the last commit
	 * @return map_t structure with last commit data
	*/
	map_t get_last_map_data();

	/*
	 * Getting the repository directory
	 * @return directory
	*/
	std::string get_repo_directory();
}