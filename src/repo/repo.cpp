#include "repo.h"

#include "../fs/fs.h"
#include "../utils/utils.h"

#include <regex>
#include <fstream>
#include <unordered_set>

static void looking_for_similars(const std::string& path)
{
	char* similars[MAX_SIMILARS];
	int   similar_num = 0;

	fs::find_similar_files(path, ".", similars, &similar_num);

	for (int i = 0; i < similar_num; i++) {
		printf("     - %s?\n", similars[i]);
	}
}

repo_t repo::initialize()
{
	repo_t ret;

	ret.status = Inactive;

	if (fs::exists(ENV_BASE_DIRECTORY).as(exist_directory) &&
		fs::exists(ENV_STORAGE_DIRECTORY).as(exist_directory))
	{
		if (fs::exists(ENV_COMMITMAP_FILENAME).as(exist_object) &&
			fs::exists(ENV_FILES_FILENAME).as(exist_object))
		{
			ret.status = Active;
		}
	}

	return ret;
}

void repo::add_object(const std::string& path)
{
	auto important_files = util::get_untracked_files().empty() && util::get_modified_files().empty();
	auto tracking_files  = util::get_files(trackingFiles);

	if (fs::exists(path).as(exist_none)) {
		printf("The file or directory at '%s' was not found\n", path.c_str());
		looking_for_similars(path);
	}

	else if (fs::exists(path).as(exist_object)) {
		for (int i = 0; i < tracking_files.size(); i++) {
			if (tracking_files[i] == path)
				return;
		}

		if (!important_files)
			util::add_object_to_files(tempFiles, path);
	}

	else if (fs::exists(path).as(exist_directory)) {
		char* files[MAX_FILES];
		int   file_num = 0;

		if (path == ".")
			fs::get_directory_files(".", files, &file_num, fm_recursive);

		else
		{
			if (fs::find_files_in_directories(utils::get_current_directory(), path)) {
				fs::get_directory_files(path, files, &file_num, fm_recursive);
			}
		}

		for (int i = 0; i < file_num; i++)
		{
			bool already_exists = false;
			for (int j = 0; j < tracking_files.size(); j++) {
				if (tracking_files[j] == files[i]) {
					already_exists = true;
					break;
				}
			}

			if (already_exists)
				continue;

			if (!important_files)
				util::add_object_to_files(tempFiles, files[i]);
		}
	}
}

void repo::remove_object(const std::string& path)
{
	return util::remove_object_from_files(tempFiles, path);
}

void repo::set_commit_message(cfg_t cfg, const std::string& msg)
{
	map_t map;

	map.hash      = "--" + utils::random_string(ENV_COMMIT_HASH_LENGTH, ENV_COMMIT_HASH_RANDOM_CHARS);
	map.timestamp = utils::timestamp::get();
	map.msg       = msg;
	map.cfg       = cfg;

	if (!util::copy_objects(map.hash) || !util::add_to_map(map)) {
		printf("Failed to add a commit to file with commit data\n");
		return;
	}

	auto temp_files = util::get_files(tempFiles);

	for (int i = 0; i < temp_files.size(); i++) {
		util::add_object_to_files(trackingFiles, temp_files[i]);
	}

	util::remove_object_from_files(tempFiles, "");
}

void repo::set_commit_action(std::string hash, commit_action action)
{
	auto map = util::get_map_list();

	if (map.empty()) {
		printf("You don't have any commit data\n");
		return;
	}

	std::unordered_set<std::string> hashes;

	for (int i = 0; i < map.size(); i++) {
		hashes.insert(map[i].hash);
	}

	if (hashes.find(hash) == hashes.end()) {
		if (hash.find_first_not_of(' ') == std::string::npos)
			hash.clear();
		printf("Hash '%s' not found --\n", hash.c_str());
		for (int i = 0; i < map.size(); i++) {
			printf("%s - %s | %s <%s>\n",
				map[i].hash.c_str(),
				utils::timestamp::fmt(map[i].timestamp).c_str(),
				map[i].cfg.username.c_str(),
				map[i].cfg.email.c_str());
		}
		return;
	}

	switch (action) {
	case returnStateCommit: {
		std::string source_path{ ENV_STORAGE_DIRECTORY "\\" + hash };
		std::string dest_path{ utils::get_current_directory() };

		fs::copy(source_path, dest_path,
			std::filesystem::copy_options::recursive |
			std::filesystem::copy_options::overwrite_existing
		);
		break;
	}
	}
}

void repo::reset_repo()
{
	fs::delete_objects({ ENV_BASE_DIRECTORY });
	util::setup_base_files();
}