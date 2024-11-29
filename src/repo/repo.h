#pragma once

#include "repo_util.h"

enum repo_stat {
	Active,
	Inactive,
	notAuthorized,
};

struct repo_t {
	repo_stat status;
};

namespace repo
{
	repo_t initialize();
	void add_object(const std::string& path);
	void remove_object(const std::string& path);
	void set_commit(cfg_t cfg, const std::string& msg);
}