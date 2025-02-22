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
	/*
	 * Repository initialization
	 * @return repo_t structure with repository status
	*/
	repo_t initialize();

	/*
	 * Adding files to the current repository
	 * @param path to the object
	*/
	void add_object(const std::string& path);

	/*
	 * Adding files to the list of ignored files (.cmapignore)
	 * @param object name
	*/
	void add_to_ignore_list(const std::string& object);

	/*
	 * Deleting a file from the list of tracked files
	 * @param path to the object
	*/
	void remove_object(const std::string& path);

	/*
	 * Create a new commit for your changes
	 * @param cfg_t structure with user data
	 * @param commit message
	*/
	void set_commit_message(cfg_t cfg, const std::string& msg);

	/*
	 * Set commit actions
	 * @param hash
	 * @param type action
	*/
	void set_commit_action(const std::string& hash, commit_action action);

	/*
	 * Deleting the current repository
	*/
	void delete_repo();
}