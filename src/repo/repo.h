#pragma once

#include "repo_util.h"

enum repo_stat {
	Active,
	Inactive,
	notAuthorized,
};

enum commit_action {
	returnStateCommit,
};

struct repo_t {
	repo_stat status;
};

namespace repo
{
	repo_t initialize();
	void add_object(const std::string& path);
	void add_to_ignore_list(const std::string& object);
	void remove_object(const std::string& path);
	void set_commit_message(cfg_t cfg, const std::string& msg);
	void set_commit_action(const std::string& hash, commit_action action);
}