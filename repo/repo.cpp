#include "repo.h"

#include "../fs/fs.h"
#include "../utils/utils.h"

static void looking_for_similars(const std::string& path)
{
	char* similars[MAX_SIMILARS];
	int   similar_num = 0;

	fs::find_similar_files(path, ".", similars, &similar_num);

	for (int i = 0; i < similar_num; i++) {
		std::cout << std::string(5, ' ') << "- " << similars[i] << "?" << std::endl;
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
	if (!util::ignore_spec_objects(path))
		goto fail;

	if (fs::exists(path).as(existNone)) {
fail:
		std::cout << "The file or directory at '" << path << "' was not found" << std::endl;
		looking_for_similars(path);
	}

	else if (fs::exists(path).as(existObject)) {
		util::add_object_to_files(tempFiles, path);
	}

	else if (fs::exists(path).as(existDirectory)) {
		char* files[MAX_FILES];
		int   file_num = 0;

		fs::get_directory_files(path, files, &file_num, fmRecursive);

		for (int i = 0; i < file_num; i++) {
			if (!util::ignore_spec_objects(files[i]))
				continue;

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

void repo::log(logging_type type)
{
	switch (type) {
	case repoLog: {
		std::vector<map_t> map = repo::util::get_map_list();
		for (const auto& obj : map) {
			std::cout << obj.hash << std::endl;
			std::cout << std::endl;
			std::cout << std::string(5, ' ') << obj.msg << std::endl;
			std::cout << std::endl;
			std::cout << std::string(obj.msg.length() + 5, ' ') << utils::timestamp::fmt(obj.timestamp) << " | ";
			std::cout << obj.cfg.username << " <" << obj.cfg.email << ">" << std::endl;
			std::cout << std::endl;
		}
		break;
	}
	case repoStatus: {
		std::vector<std::string> _temporary_files = repo::util::get_files(tempFiles);
		std::vector<std::string> _untracked_files = repo::util::get_untracked_files();

		if (!_temporary_files.empty()) {
			std::cout << "You committing next files:" << std::endl;
			for (const auto& f : _temporary_files) {
				std::cout << std::string(5, ' ') << f << std::endl;
			}
		}

		if (!_untracked_files.empty()) {
			std::cout << "Untracked files:" << std::endl;
			for (const auto& f : _untracked_files) {
				std::cout << std::string(5, ' ') << f << std::endl;
			}
			std::cout << std::string(2, ' ') << "-- To add files type \"cmap add <filename>\" --" << std::endl;
		}
		break;
	}
	}
}