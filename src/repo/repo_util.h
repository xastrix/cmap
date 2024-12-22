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
	base_files_stat setup_base_files();
	std::vector<std::string> get_files(const file_type type);
	std::vector<std::string> get_untracked_files();
	std::vector<std::string> get_modified_files();
	std::vector<std::string> get_deleted_files();
	bool copy_objects(const std::string& hash);
	bool add_to_map(const map_t map);
	void add_object_to_files(const file_type type, const std::string& obj);
	void remove_object_from_files(const file_type type, const std::string& obj);
	bool is_ignore_file_detected(const std::string& filename);
	std::vector<map_t> get_map_list();
	map_t get_last_map_data();
	std::string get_repo_directory();
}