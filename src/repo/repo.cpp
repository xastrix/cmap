#include "repo.h"

#include "../fs/fs.h"
#include "../utils/utils.h"
#include "../fmt/fmt.h"

#include <regex>
#include <fstream>

static void looking_for_similars(const std::string& path)
{
	char* similars[MAX_SIMILARS];
	int   similar_num = 0;

	fs::find_similar_files(path, ".", similars, &similar_num);

	for (int i = 0; i < similar_num; i++) {
		if (strncmp(similars[i], ENV_IGNORE_LIST_FILENAME, strlen(ENV_BASE_DIRECTORY)) == 0)
			continue;

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
	auto noReasonToAdding = util::get_untracked_files().empty() && util::get_modified_files().empty();
	auto tracking_files   = util::get_files(trackingFiles);

	std::regex base_directory_pattern{ ENV_BASE_DIRECTORY_PATTERN };

	if (fs::exists(path).as(existNone) || std::regex_match(path, base_directory_pattern)) {
		fmt{ fc_none, "The file or directory at '%s' was not found\n", path.c_str() };
		looking_for_similars(path);
	}

	else if (fs::exists(path).as(existObject)) {
		if (util::is_ignore_file_detected(path))
			return;

		for (int i = 0; i < tracking_files.size(); i++) {
			if (tracking_files[i] == path)
				return;
		}

		if (!noReasonToAdding)
			util::add_object_to_files(tempFiles, path);
	}

	else if (fs::exists(path).as(existDirectory)) {
		if (util::is_ignore_file_detected(path))
			return;

		char* files[MAX_FILES];
		int   file_num = 0;

		if (path == ".")
			fs::get_directory_files(".", files, &file_num, fmRecursive);

		else
		{
			if (fs::find_files_in_directories(utils::get_current_directory(), path)) {
				fs::get_directory_files(path, files, &file_num, fmRecursive);
			}
		}

		for (int i = 0; i < file_num; i++)
		{
			if (util::is_ignore_file_detected(files[i]))
				continue;

			if (std::regex_match(files[i], base_directory_pattern))
				continue;

			bool already_exists = false;
			for (int j = 0; j < tracking_files.size(); j++) {
				if (tracking_files[j] == files[i]) {
					already_exists = true;
					break;
				}
			}

			if (already_exists)
				continue;

			if (!noReasonToAdding)
				util::add_object_to_files(tempFiles, files[i]);
		}
	}
}

void repo::add_to_ignore_list(const std::string& object)
{
	if (!util::is_ignore_file_detected(object))
		fs::make_file(ENV_IGNORE_LIST_FILENAME, object, std::ios::app);
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
		fmt{ fc_none, "Failed to add a commit to file with commit data\n" };
		return;
	}

	auto temp_files = util::get_files(tempFiles);

	for (int i = 0; i < temp_files.size(); i++) {
		util::add_object_to_files(trackingFiles, temp_files[i]);
	}

	json_object object = utils::parse_json(ENV_FILES_FILENAME);

	object.ptr[ENV_TEMP_FILES_MEMBER_NAME] = Json::arrayValue;
	fs::make_file(ENV_FILES_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.ptr));
}

void repo::set_commit_action(const std::string& hash, commit_action action)
{
	auto map = util::get_map_list();

	if (map.empty()) {
		fmt{ fc_none, "You don't have any commit data\n" };
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