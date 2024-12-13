#include "cfg.h"

#include "../config.h"
#include "../fs/fs.h"
#include "../utils/utils.h"

#include <fstream>
#include <sstream>

static cfg_t read_values_from_configuration_file()
{
	cfg_t cfg;

	std::ifstream f{ utils::get_user_directory() + "\\" ENV_CONFIGURATION_FILENAME };

	if (!f.is_open()) {
		return cfg;
	}

	std::string line, content;
	while (std::getline(f, line)) {
		content += line + "\n";
	}

	f.close();

	std::stringstream ss{ content };

	while (std::getline(ss, line)) {
		if (line.empty() || line[0] == '#') {
			continue;
		}

		size_t equal_pos = line.find('=');
		if (equal_pos == std::string::npos) {
			continue;
		}

		std::string key = line.substr(0, equal_pos);
		std::string value = line.substr(equal_pos + 1);

		value = value.substr(1, value.length() - 2);

		if (key == "username") {
			cfg.username = value;
		}

		else if (key == "email") {
			cfg.email = value;
		}
	}

	return cfg;
}

static bool set_config_values(cfg_t data)
{
	if (data.username.empty() || data.email.empty())
		return false;

	char fields[512];
	sprintf(fields,
		"# Is the configuration file of cmap (version-control-system)\n"
		"# https://github.com/xastrix/cmap\n\n"

		"# You can change the configuration file manually or\n"
		"# by writing the following commands:\n"
		"# cmap config <YourUsername> <YourEmail> --user-config\n\n"

		"username=\"%s\"\n"
		"email=\"%s\"",
		data.username.c_str(), data.email.c_str()
	);

	return fs::make_file(utils::get_user_directory() + "\\" ENV_CONFIGURATION_FILENAME, fields);
}

cfg_t cfg::initialize()
{
	cfg_t cfg;

	cfg.username = "?";
	cfg.email    = "?";

	if (!fs::exists(utils::get_user_directory() + "\\" ENV_CONFIGURATION_FILENAME).as(existObject))
		set_config_values(cfg);

	return cfg = read_values_from_configuration_file();
}

bool cfg::update_cfg(cfg_t data)
{
	return set_config_values(data);
}