#include "repo_util.h"

#include "../fs/fs.h"
#include "../utils/utils.h"

#include <regex>
#include <fstream>

base_files_stat repo::util::setup_base_files()
{
	base_files_stat ret = InitializeNew;

	if (fs::exists(ENV_BASE_DIRECTORY).as(exist_directory) &&
		fs::exists(ENV_STORAGE_DIRECTORY).as(exist_directory) &&
		fs::exists(ENV_COMMITMAP_FILENAME).as(exist_object) &&
		fs::exists(ENV_FILES_FILENAME).as(exist_object))
	{
		ret = alreadyExists;
		return ret;
	}

	if (fs::make_directory(ENV_BASE_DIRECTORY, da_hidden) == fsm_perm_denied)
		ret = permissionDenied;

	if (fs::make_directory(ENV_STORAGE_DIRECTORY) == fsm_perm_denied)
		ret = permissionDenied;

	if (fs::make_file(ENV_COMMITMAP_FILENAME) == fsm_perm_denied)
		ret = permissionDenied;

	if (fs::make_file(ENV_FILES_FILENAME, "{\"Tracking\":[],\"Temp\":[]}") == fsm_perm_denied)
		ret = permissionDenied;

	return ret;
}

std::vector<std::string> repo::util::get_files(const file_type type)
{
	json_object object = utils::parse_json(ENV_FILES_FILENAME);
	std::vector<std::string> _files;

	switch (type) {
	case trackingFiles: {
		for (int i = 0; i < object.ptr[ENV_TRACKING_FILES_MEMBER_NAME].size(); i++) {
			_files.push_back(object.ptr[ENV_TRACKING_FILES_MEMBER_NAME][i].asString());
		}
		break;
	}
	case tempFiles: {
		for (int i = 0; i < object.ptr[ENV_TEMP_FILES_MEMBER_NAME].size(); i++) {
			_files.push_back(object.ptr[ENV_TEMP_FILES_MEMBER_NAME][i].asString());
		}
		break;
	}
	}

	return _files;
}

std::vector<std::string> repo::util::get_untracked_files()
{
	char* current_files[MAX_FILES];
	int   current_file_num = 0;

	fs::get_directory_files(".", current_files, &current_file_num, fm_recursive);

	auto tracking_files = get_files(trackingFiles);
	auto temp_files = get_files(tempFiles);

	std::vector<std::string> _untracked_files;

	for (int i = 0; i < current_file_num; i++)
	{
		bool already_exists = false;

		for (int k = 0; k < tracking_files.size(); k++) {
			if (strcmp(utils::to_lower(tracking_files[k]).c_str(), utils::to_lower(current_files[i]).c_str()) == 0) {
				already_exists = true;
				break;
			}
		}

		for (int j = 0; j < temp_files.size(); j++) {
			if (strcmp(utils::to_lower(temp_files[j]).c_str(), utils::to_lower(current_files[i]).c_str()) == 0) {
				already_exists = true;
				break;
			}
		}

		if (!already_exists)
			_untracked_files.push_back(current_files[i]);
	}

	return _untracked_files;
}

std::vector<std::string> repo::util::get_modified_files()
{
	auto tracking_files = get_files(trackingFiles);
	auto temp_files     = get_files(tempFiles);

	std::vector<std::string> _modified_files;

	for (int i = 0; i < tracking_files.size(); i++)
	{
		bool already_exists = false;
		for (int j = 0; j < temp_files.size(); j++) {
			if (strcmp(utils::to_lower(tracking_files[i]).c_str(), utils::to_lower(temp_files[j]).c_str()) == 0) {
				already_exists = true;
				break;
			}
		}

		if (already_exists)
			continue;

		std::string tracked_path{ tracking_files[i] };

		if (fs::exists(tracked_path).as(exist_object))
		{
			std::string file_path{ ENV_STORAGE_DIRECTORY "\\" + get_last_map_data().hash + "\\" + tracked_path };

			if (fs::exists(file_path).as(exist_object))
			{
				if (fs::get_file_content(tracked_path) != fs::get_file_content(file_path))
				{
					_modified_files.push_back(tracking_files[i]);
				}
			}
		}
	}

	return _modified_files;
}

std::vector<std::string> repo::util::get_deleted_files()
{
	auto tracking_files = get_files(trackingFiles);

	std::vector<std::string> _deleted_files;

	for (int i = 0; i < tracking_files.size(); i++)
	{
		std::string file_path{ ENV_STORAGE_DIRECTORY "\\" + get_last_map_data().hash + "\\" + tracking_files[i] };

		if (fs::exists(file_path).as(exist_object))
		{
			if (!fs::exists(tracking_files[i]).as(exist_object))
			{
				_deleted_files.push_back(tracking_files[i]);
			}
		}
	}

	return _deleted_files;
}

bool repo::util::copy_objects(const std::string& hash)
{
	std::string commit_directory{ ENV_STORAGE_DIRECTORY "\\" + hash };

	if (!fs::make_directory(commit_directory))
		return false;

	json_object object = utils::parse_json(ENV_FILES_FILENAME);

	for (int i = 0; i < object.ptr[ENV_TEMP_FILES_MEMBER_NAME].size(); i++)
	{
		if (!fs::exists(object.ptr[ENV_TEMP_FILES_MEMBER_NAME][i].asString()).as(exist_none))
		{
			std::string source{ object.ptr[ENV_TEMP_FILES_MEMBER_NAME][i].asString() };
			std::string destination{ commit_directory + "\\" + object.ptr[ENV_TEMP_FILES_MEMBER_NAME][i].asString() };

			fs::copy(source, destination, std::filesystem::copy_options::recursive);
		}
	}

	for (int j = 0; j < object.ptr[ENV_TRACKING_FILES_MEMBER_NAME].size(); j++)
	{
		if (!fs::exists(object.ptr[ENV_TRACKING_FILES_MEMBER_NAME][j].asString()).as(exist_none))
		{
			std::string source{ object.ptr[ENV_TRACKING_FILES_MEMBER_NAME][j].asString() };
			std::string destination{ commit_directory + "\\" + object.ptr[ENV_TRACKING_FILES_MEMBER_NAME][j].asString() };

			fs::copy(source, destination, std::filesystem::copy_options::recursive);
		}
	}

	return true;
}

bool repo::util::add_to_map(const map_t map)
{
	json_object object = utils::parse_json(ENV_COMMITMAP_FILENAME);

	object.ptr[map.hash]["msg"]                   = map.msg;
	object.ptr[map.hash]["committer"]["username"] = map.cfg.username;
	object.ptr[map.hash]["committer"]["email"]    = map.cfg.email;
	object.ptr[map.hash]["timestamp"]             = map.timestamp;

	if (!fs::make_file(ENV_COMMITMAP_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.ptr)))
		return false;

	return true;
}

void repo::util::add_object_to_files(const file_type type, const std::string& obj)
{
	std::string member_name{};

	if (type == tempFiles)
		member_name = ENV_TEMP_FILES_MEMBER_NAME;

	else if (type == trackingFiles)
		member_name = ENV_TRACKING_FILES_MEMBER_NAME;

	json_object object = utils::parse_json(ENV_FILES_FILENAME);

	for (int i = 0; i < object.ptr[member_name].size(); i++)
	{
		if (object.ptr[member_name][i].asString() == obj) {
			return;
		}
	}

	object.ptr[member_name].append(obj);
	fs::make_file(ENV_FILES_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.ptr));
}

void repo::util::remove_object_from_files(const file_type type, const std::string& obj)
{
	std::string member_name{};

	if (type == tempFiles)
		member_name = ENV_TEMP_FILES_MEMBER_NAME;

	else if (type == trackingFiles)
		member_name = ENV_TRACKING_FILES_MEMBER_NAME;

	json_object object = utils::parse_json(ENV_FILES_FILENAME);

	switch (obj.empty()) {
	case true: {
		object.ptr[member_name] = Json::arrayValue;
		break;
	}
	case false: {
		for (auto& it = object.ptr[member_name].begin();
			it != object.ptr[member_name].end(); ++it)
		{
			if (it->asString() == obj) {
				object.ptr[member_name].removeIndex(it.index(), nullptr);
			}
		}
		break;
	}
	}

	fs::make_file(ENV_FILES_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.ptr));
}

std::vector<map_t> repo::util::get_map_list()
{
	json_object object = utils::parse_json(ENV_COMMITMAP_FILENAME);
	std::vector<map_t> _map_list;

	for (int i = 0; i < object.ptr.getMemberNames().size(); i++) {
		map_t map;
		cfg_t cfg;

		map.hash      = object.ptr.getMemberNames()[i];
		map.msg       = object.ptr[object.ptr.getMemberNames()[i]]["msg"].asString();
		map.timestamp = object.ptr[object.ptr.getMemberNames()[i]]["timestamp"].asLargestUInt();
		cfg.username  = object.ptr[object.ptr.getMemberNames()[i]]["committer"]["username"].asString();
		cfg.email     = object.ptr[object.ptr.getMemberNames()[i]]["committer"]["email"].asString();
		map.cfg       = cfg;

		if (fs::find_files_in_directories(ENV_STORAGE_DIRECTORY, map.hash))
			_map_list.push_back(map);
	}

	return _map_list;
}

map_t repo::util::get_last_map_data()
{
	auto map = get_map_list();
	map_t ret;

	if (map.empty())
		return ret;

	ret = map[0];

	for (int i = 0; i < map.size(); i++)
	{
		if (map[i].timestamp > ret.timestamp) {
			ret = map[i];
		}
	}

	return ret;
}

std::string repo::util::get_repo_directory()
{
	return utils::get_current_directory();
}