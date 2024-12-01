#include "repo.h"

#include "../fs/fs.h"
#include "../utils/utils.h"
#include "../fmt/fmt.h"

static void looking_for_similars(const std::string& path)
{
	char* similars[MAX_SIMILARS];
	int   similar_num = 0;

	fs::find_similar_files(path, ".", similars, &similar_num);

	for (int i = 0; i < similar_num; i++) {
		fmt{ fc_none, "%s - %s?\n", std::string(5, ' ').c_str(), similars[i] };
	}
}

repo_t repo::initialize()
{
	repo_t ret;

	ret.status = Inactive;

	if (fs::exists(ENV_BASE_DIRECTORY).as(existDirectory) &&
		fs::exists(ENV_STORAGE_DIRECTORY).as(existDirectory))
	{
		if (fs::exists(ENV_COMMITMAP_FILENAME).as(existObject) &&
			fs::exists(ENV_FILES_FILENAME).as(existObject))
		{
			ret.status = Active;
		}
	}

	return ret;
}

void repo::add_object(const std::string& path)
{
	bool noReasonToAdding = util::get_untracked_files().empty() && util::get_modified_files().empty();

	if (fs::exists(path).as(existNone) || path.substr(0, std::string{ ENV_BASE_DIRECTORY }.length()) == ENV_BASE_DIRECTORY) {
		fmt{ fc_none, "The file or directory at '%s' was not found\n", path.c_str() };
		looking_for_similars(path);
	}

	else if (fs::exists(path).as(existObject)) {
		if (!noReasonToAdding)
			util::add_object_to_files(tempFiles, path);
	}

	else if (fs::exists(path).as(existDirectory)) {
		char* files[MAX_FILES];
		int   file_num = 0;

		fs::get_directory_files(path, files, &file_num, fmRecursive);

		for (int i = 0; i < file_num; i++) {
			if (std::string{ files[i] }.substr(0, std::string{ ENV_BASE_DIRECTORY }.length()) == ENV_BASE_DIRECTORY)
				continue;

			if (!noReasonToAdding)
				repo::util::add_object_to_files(tempFiles, files[i]);
		}
	}
}

void repo::remove_object(const std::string& path)
{
	return util::remove_object_from_files(tempFiles, path);
}

void repo::set_commit(cfg_t cfg, const std::string& msg)
{
	map_t map;

	map.hash      = "--" + utils::random_string(ENV_COMMIT_HASH_LENGTH, ENV_COMMIT_HASH_RANDOM_CHARS);
	map.timestamp = utils::timestamp::get();
	map.msg       = msg;
	map.cfg       = cfg;

	if (util::add_to_map(map) && util::copy_objects(map.hash))
	{
		for (int i = 0; i < util::get_files(tempFiles).size(); i++) {
			util::add_object_to_files(trackingFiles, util::get_files(tempFiles)[i]);
		}

		util::clear_temp_files();
	}
}

void repo::revert_commit(const std::string& hash)
{
	auto map = util::get_map_list();

	if (map.empty()) {
		fmt{ fc_none, "You dont have any commit.\n" };
		return;
	}

	bool hash_founded = false;
	for (int i = 0; i < map.size(); i++) {
		if (map[i].hash == hash) {
			hash_founded = true;
			break;
		}
	}

	if (!hash_founded) {
		fmt{ fc_none, "Hash '%s' not found --\n", hash.c_str() };
		for (int i = 0; i < map.size(); i++) {
			fmt{ fc_none, "%s - %s\n", map[i].hash.c_str(), utils::timestamp::fmt(map[i].timestamp).c_str() };
		}
		return;
	}

	std::string source_path{ ENV_STORAGE_DIRECTORY "\\" + hash };
	std::string dest_path{ utils::get_current_directory() + "\\" };

	fs::copy(source_path, dest_path,
		std::filesystem::copy_options::recursive |
		std::filesystem::copy_options::overwrite_existing
	);
}