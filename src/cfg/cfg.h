#pragma once

#include <string>

struct cfg_t {
	std::string username;
	std::string email;
};

namespace cfg
{
	cfg_t initialize();
	bool update_cfg(cfg_t data);
}