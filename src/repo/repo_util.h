#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../config.h"
#include "../cfg/cfg.h"

struct map_t {
	std::string hash, msg;
	cfg_t cfg;
	long long timestamp;
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
	bool add_to_map(map_t map);
	void add_object_to_files(const file_type type, const std::string& obj);
	void remove_object_from_files(const file_type type, const std::string& obj);
	std::vector<map_t> get_map_list();
	std::string get_repo_directory();
}