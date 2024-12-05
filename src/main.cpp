#include "cli/cli.h"
#include "repo/repo.h"
#include "utils/utils.h"
#include "fmt/fmt.h"

int main(int argc, const char** argv)
{
	cli cli{
		"cmap a micro version system control\n\n"

		"cmap init\n"
		"  Initialize a new repository. This sets up the necessary structure for tracking file changes.\n\n"

		"cmap set\n"
		"  Changes some user-defined settings.\n"
		"    Sub-commands:\n"
		"      Sub-command    Description         Example\n"
		"      --user-config  Changing user data  cmap set \"username\" \"mail\" --user-config\n"
		"\n"

		"cmap add <filename>\n"
		"  Add files to the tracked file list. (You can use . to track all files in the current directory)\n"
		"  Also, you can delete a file that was added accidentally, type 'cmap add <filename> --rm'\n\n"

		"cmap commit <msg>\n"
		"  Create a new commit for your changes. This saves the current state of your tracked files.\n\n"

		"cmap revert <hash>\n"
		"  Returns the state of the repository to the specified commit, identified by the hash.\n\n"

		"cmap status\n"
		"  Display the status of your files, showing which are tracked or untracked.\n\n"

		"cmap log\n"
		"  Retrieve information about past commits, including messages and timestamps.\n"
	};

	cfg_t cfg{ cfg::initialize() };
	repo_t repo{ repo::initialize() };

	if (cfg.username == "?" && cfg.email == "?")
		repo.status = notAuthorized;

	cli.add("init", [&](int ac, arguments_t args) {
		if (repo.status == notAuthorized) {
			fmt{ fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config\n" };
			fmt{ fc_none, "  (You can't initialize repository without user data)\n" };
			return;
		}

		if (repo::util::setup_base_files() == alreadyExists)
			repo.status = Active;

		fmt{ fc_none, "Repository successful %s\n",
			repo.status == Inactive ? "initialized" : "reinitialized" };

		fmt{ fc_none, "  (%s)\n", repo::util::get_repo_directory().c_str() };
	});

	cli.add("set", [&](int ac, arguments_t args) {
		if (ac != 3 || args[3] != "--user-config")
			return;

		cfg_t cfg;

		cfg.username = args[1];
		cfg.email = args[2];

		if (!cfg::update_cfg(cfg)) {
			fmt{ fc_none, "Failed to update configuration file\n" };
		}
	});

	cli.add("add", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			if (ac == 1)
				repo::add_object(args[1]);
			else if (ac == 2 && args[2] == "--rm")
				repo::remove_object(args[1]);
			break;
		}
		case Inactive: {
			fmt{ fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config\n" };
			fmt{ fc_none, "  (You can't add files without user data)\n" };
			break;
		}
		}
	});

	cli.add("commit", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			auto temp_files = repo::util::get_files(tempFiles);
			auto modified_files = repo::util::get_modified_files();

			if (temp_files.empty() && modified_files.empty()) {
				fmt{ fc_none, "Add files to the staging area before committing\n" };
				break;
			}

			if (ac != 1 || args[1].empty()) {
				fmt{ fc_none, "Commit message is empty\n" };
				break;
			}

			repo::set_commit_message(cfg, args[1]);
			break;
		}
		case Inactive: {
			fmt{ fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config\n" };
			fmt{ fc_none, "  (You can't committing without user data)\n" };
			break;
		}
		}
	});

	cli.add("revert", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			if (ac != 1 || args[1].empty()) {
				fmt{ fc_none, "Hash is empty\n" };
				break;
			}

			repo::set_commit_action(args[1], revertCommit);
			break;
		}
		case Inactive: {
			fmt{ fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config\n" };
			fmt{ fc_none, "  (You can't revert without user data)\n" };
			break;
		}
		}
	});

	cli.add("status", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			auto _temporary_files = repo::util::get_files(tempFiles);
			auto _untracked_files = repo::util::get_untracked_files();
			auto _modified_files  = repo::util::get_modified_files();
			auto _deleted_files   = repo::util::get_deleted_files();

			if (!_temporary_files.empty()) {
				fmt{ fc_none, "You committing next files --\n" };
				for (const auto& f : _temporary_files) {
					fmt{ fc_green, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (!_untracked_files.empty()) {
				fmt{ fc_none, "Untracked files --\n" };
				for (const auto& f : _untracked_files) {
					fmt{ fc_none, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (!_modified_files.empty()) {
				fmt{ fc_none, "Modified files --\n" };
				for (const auto& f : _modified_files) {
					fmt{ fc_yellow, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (!_deleted_files.empty()) {
				fmt{ fc_none, "Deleted files --\n" };
				for (const auto& f : _deleted_files) {
					fmt{ fc_red, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (_temporary_files.empty() && _untracked_files.empty() &&
				_modified_files.empty() && _deleted_files.empty())
				fmt{ fc_none, "There is no change. Modify files.\n" };
			break;
		}
		case Inactive: {
			fmt{ fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config\n" };
			fmt{ fc_none, "  (You can't check status without user data)\n" };
			break;
		}
		}
	});

	cli.add("log", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			auto map = repo::util::get_map_list();

			std::sort(map.begin(), map.end(), [](const map_t& a, const map_t& b) {
				return a.timestamp > b.timestamp;
			});

			for (int i = 0; i < map.size(); i++) {
				fmt{ fc_none, "%s\n\n", map[i].hash.c_str() };
				fmt{ fc_none, "%s%s\n\n", std::string(5, ' ').c_str(), map[i].msg.c_str() };
				fmt{ fc_none, "%s%s | %s <%s>\n\n",
					std::string(map[i].msg.length() + 5, ' ').c_str(),
					utils::timestamp::fmt(map[i].timestamp).c_str(),
					map[i].cfg.username.c_str(),
					map[i].cfg.email.c_str() };
			}
			break;
		}
		case Inactive: {
			fmt{ fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config\n" };
			fmt{ fc_none, "  (You can't check logs without user data)\n" };
			break;
		}
		}
	});

	return cli.parse(argc, argv);
}