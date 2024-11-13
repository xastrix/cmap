#include "repo_util.h"

#include "../fs/fs.h"
#include "../utils/utils.h"

#include <json/json.h>

struct json_object {
	json_object() : res(true) {}
	Json::Value value;
	bool res;
};

static json_object parse_json(const std::string& content)
{
	json_object ret;

	Json::CharReaderBuilder reader_builder{};
	std::istringstream ss{ content };
	Json::String log{};

	if (!Json::parseFromStream(reader_builder, ss, &ret.value, &log)) {
		ret.res = false;
		return ret;
	}

	return ret;
}

base_files_stat repo::util::setup_base_files()
{
	base_files_stat ret = setupNews;

	if (!fs::make_directory(ENV_BASE_DIRECTORY, 2))
		if (fs::exists(ENV_BASE_DIRECTORY).as(existDirectory))
			ret = alreadyExists;

	if (!fs::make_directory(ENV_STORAGE_DIRECTORY))
		if (fs::exists(ENV_STORAGE_DIRECTORY).as(existDirectory))
			ret = alreadyExists;

	if (!fs::make_file(ENV_COMMITMAP_FILENAME))
		if (fs::exists(ENV_COMMITMAP_FILENAME).as(existObject))
			ret = alreadyExists;

	if (!fs::make_file(ENV_FILES_FILENAME, "{\"Tracking\":[],\"Temp\":[]}"))
		if (fs::exists(ENV_FILES_FILENAME).as(existObject))
			ret = alreadyExists;

	return ret;
}

std::vector<std::string> repo::util::get_files(const file_type type)
{
	std::vector<std::string> _files;
	json_object object = parse_json(fs::get_file_content(ENV_FILES_FILENAME));

	switch (type) {
	case trackingFiles: {
		for (int i = 0; i < object.value["Tracking"].size(); i++) {
			_files.push_back(object.value["Tracking"][i].asString());
		}
		break;
	}
	case tempFiles: {
		for (int i = 0; i < object.value["Temp"].size(); i++) {
			_files.push_back(object.value["Temp"][i].asString());
		}
		break;
	}
	}

	return _files;
}

std::vector<std::string> repo::util::get_untracked_files()
{
	std::vector<std::string> _untracked_files;

	char* current_files[MAX_FILES];
	int   current_file_num = 0;

	fs::get_directory_files(".", current_files, &current_file_num, fmRecursive);

	for (int i = 0; i < current_file_num; i++)
	{
		if (!ignore_spec_objects(current_files[i]))
			continue;

		bool already_exists = false;
		for (int k = 0; k < get_files(trackingFiles).size(); k++) {
			if (strcmp(get_files(trackingFiles)[k].c_str(), current_files[i]) == 0) {
				already_exists = true;
				break;
			}
		}

		for (int j = 0; j < get_files(tempFiles).size(); j++) {
			if (strcmp(get_files(tempFiles)[j].c_str(), current_files[i]) == 0) {
				already_exists = true;
				break;
			}
		}

		if (!already_exists)
			_untracked_files.push_back(current_files[i]);
	}

	return _untracked_files;
}

bool repo::util::copy_objects(const std::string& hash)
{
	json_object object = parse_json(fs::get_file_content(ENV_FILES_FILENAME));
	std::string commit_directory{ ENV_STORAGE_DIRECTORY "/" + hash };

	if (!fs::make_directory(commit_directory))
		return false;

	for (int i = 0; i < object.value["Temp"].size(); i++)
	{
		if (!fs::exists(object.value["Temp"][i].asString()).as(existNone))
		{
			std::string source{ object.value["Temp"][i].asString() };
			std::string destination{ commit_directory + "/" + object.value["Temp"][i].asString() };

			fs::copy(source, destination, std::filesystem::copy_options::recursive);
		}
	}

	return true;
}

bool repo::util::add_to_map(map_t map)
{
	json_object object = parse_json(fs::get_file_content(ENV_COMMITMAP_FILENAME));

	object.value[map.hash]["msg"] = map.msg;
	object.value[map.hash]["committer"]["username"] = map.cfg.username;
	object.value[map.hash]["committer"]["email"] = map.cfg.email;
	object.value[map.hash]["timestamp"] = map.timestamp;

	if (!fs::make_file(ENV_COMMITMAP_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.value)))
		return false;

	return true;
}

void repo::util::add_object_to_files(const file_type type, const std::string& obj)
{
	json_object object = parse_json(fs::get_file_content(ENV_FILES_FILENAME));
	std::string member_name{};

	if (type == tempFiles)
		member_name = "Temp";
	if (type == trackingFiles)
		member_name = "Tracking";

	for (int i = 0; i < object.value[member_name].size(); i++) {
		if (object.value[member_name][i].asString() == obj) {
			return;
		}
	}

	object.value[member_name].append(obj);
	fs::make_file(ENV_FILES_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.value));
}

void repo::util::remove_object_from_files(const file_type type, const std::string& obj)
{
	json_object object = parse_json(fs::get_file_content(ENV_FILES_FILENAME));
	std::string member_name{};

	if (type == tempFiles)
		member_name = "Temp";
	if (type == trackingFiles)
		member_name = "Tracking";

	for (Json::ValueIterator& it = object.value[member_name].begin();
		it != object.value[member_name].end(); ++it)
	{
		if (it->asString() == obj) {
			object.value[member_name].removeIndex(it.index(), nullptr);
		}
	}

	fs::make_file(ENV_FILES_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.value));
}

void repo::util::clear_temp_files()
{
	json_object object = parse_json(fs::get_file_content(ENV_FILES_FILENAME));

	object.value["Temp"] = Json::arrayValue;
	fs::make_file(ENV_FILES_FILENAME, Json::writeString(Json::StreamWriterBuilder{}, object.value));
}

std::vector<map_t> repo::util::get_map_list()
{
	std::vector<map_t> _map_list;
	json_object object = parse_json(fs::get_file_content(ENV_COMMITMAP_FILENAME));

	for (int i = 0; i < object.value.getMemberNames().size(); i++) {
		map_t map;
		cfg_t cfg;

		map.hash      = object.value.getMemberNames()[i];
		map.msg       = object.value[object.value.getMemberNames()[i]]["msg"].asString();
		map.timestamp = object.value[object.value.getMemberNames()[i]]["timestamp"].asLargestUInt();
		cfg.username  = object.value[object.value.getMemberNames()[i]]["committer"]["username"].asString();
		cfg.email     = object.value[object.value.getMemberNames()[i]]["committer"]["email"].asString();
		map.cfg       = cfg;

		if (fs::find_files_in_directories(ENV_STORAGE_DIRECTORY, map.hash))
			_map_list.push_back(map);
	}

	return _map_list;
}

std::string repo::util::get_repo_directory()
{
	return utils::get_current_directory() + "\\" ENV_BASE_DIRECTORY;
}

bool repo::util::ignore_spec_objects(const std::string& object)
{
	if (std::string{ object }.find(ENV_BASE_DIRECTORY "\\") != std::string::npos)
		return false;

	if (std::string{ object }.find(ENV_BASE_DIRECTORY "/") != std::string::npos)
		return false;

	return true;
}