#pragma once

#include <string>

struct cfg_t {
	std::string username;
	std::string email;
};

namespace cfg
{
	/*
	 * Initialization of the configuration file with user settings
	 * @return cfg_t structure with user data
	*/
	cfg_t initialize();

	/*
	 * Updating user settings in the configuration file
	 * @param cfg_t structure with user data
	 * @example
	 *  cfg_t cfg;
	 *
	 *	cfg.username = "Your new username";
	 *	cfg.email    = "Your new email";
	 *
	 *	if (cfg::update_cfg(cfg)) ...
	 *
	 * @return true/false
	*/
	bool update_cfg(cfg_t data);
}