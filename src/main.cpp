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

		"cmap add <filename>\n"
		"  Add files to the tracked file list. (You can use . to track all files in the current directory)\n"
		"  Also, you can delete a file that was added accidentally, type 'cmap add <filename> --rm'\n\n"

		"cmap commit <msg>\n"
		"  Create a new commit for your changes. This saves the current state of your tracked files.\n\n"

		"cmap status\n"
		"  Display the status of your files, showing which are tracked or untracked.\n\n"

		"cmap log\n"
		"  Retrieve information about past commits, including messages and timestamps.\n\n"

		"cmap includes all the standard features of a simple version control system\n"
	};

	cfg_t cfg{ cfg::initialize() };
	repo_t repo{ repo::initialize() };

	if (cfg.username == "?" && cfg.email == "?")
		repo.status = notAuthorized;

	cli.add("init", [&](int ac, arguments_t args) {
		if (repo.status == notAuthorized) {
			fmt{ fmt_15ms, fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config" };
			fmt{ fmt_def, fc_none, "  (You can't initialize repository without user data)\n" };
			return;
		}

		if (repo::util::setup_base_files() == alreadyExists)
			repo.status = Active;

		fmt{ fmt_15ms, fc_none, "Repository successful %s",
			repo.status == Inactive ? "initialized" : "reinitialized" };

		fmt{ fmt_def, fc_none, "  (%s)\n", repo::util::get_repo_directory().c_str() };
	});

	cli.add("set", [&](int ac, arguments_t args) {
		if (ac == 3 && args[3] == "--user-config") {
			cfg_t cfg;

			cfg.username = args[1];
			cfg.email = args[2];

			cfg::update_cfg(cfg);
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
			fmt{ fmt_15ms, fc_none, "You need initialize repository" };
			break;
		}
		case notAuthorized: {
			fmt{ fmt_15ms, fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config" };
			fmt{ fmt_def, fc_none, "  (You can't add files without user data)\n" };
			break;
		}
		}
	});

	cli.add("commit", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			if (repo::util::get_files(tempFiles).empty()) {
				fmt{ fmt_15ms, fc_none, "Add files to the staging area before committing" };
				break;
			}

			if (ac == 1)
				repo::set_commit(cfg, args[1]);
			break;
		}
		case Inactive: {
			fmt{ fmt_15ms, fc_none, "You need initialize repository" };
			break;
		}
		case notAuthorized: {
			fmt{ fmt_15ms, fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config" };
			fmt{ fmt_def, fc_none, "  (You can't committing without user data)\n" };
			break;
		}
		}
	});

	cli.add("status", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			std::vector<std::string> _temporary_files = repo::util::get_files(tempFiles);
			std::vector<std::string> _untracked_files = repo::util::get_untracked_files();
			std::vector<std::string> _modified_files = repo::util::get_modified_files();
			std::vector<std::string> _deleted_files = repo::util::get_deleted_files();

			if (!_temporary_files.empty()) {
				fmt{ fmt_def, fc_none, "You committing next files\n" };
				for (const auto& f : _temporary_files) {
					fmt{ fmt_def, fc_green, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (!_untracked_files.empty()) {
				fmt{ fmt_def, fc_none, "Untracked files\n" };
				for (const auto& f : _untracked_files) {
					fmt{ fmt_def, fc_none, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (!_modified_files.empty()) {
				fmt{ fmt_def, fc_none, "Modified files\n" };
				for (const auto& f : _modified_files) {
					fmt{ fmt_def, fc_yellow, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (!_deleted_files.empty()) {
				fmt{ fmt_def, fc_none, "Deleted files\n" };
				for (const auto& f : _deleted_files) {
					fmt{ fmt_def, fc_red, "%s%s\n", std::string(5, ' ').c_str(), f.c_str() };
				}
			}

			if (_temporary_files.empty() && _untracked_files.empty() &&
				_modified_files.empty() && _deleted_files.empty())
				fmt{ fmt_15ms, fc_none, "There is no change. Modify files." };
			break;
		}
		case Inactive: {
			fmt{ fmt_def, fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fmt_15ms, fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config" };
			fmt{ fmt_def, fc_none, "  (You can't check status without user data)\n" };
			break;
		}
		}
	});

	cli.add("log", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			auto map = repo::util::get_map_list();
			for (int i = 0; i < map.size(); i++) {
				fmt{ fmt_def, fc_none, "%s\n\n", map[i].hash.c_str() };
				fmt{ fmt_def, fc_none, "%s%s\n\n", std::string(5, ' ').c_str(), map[i].msg.c_str() };
				fmt{ fmt_def, fc_none, "%s%s | %s <%s>\n\n",
					std::string(map[i].msg.length() + 5, ' ').c_str(),
					utils::timestamp::fmt(map[i].timestamp).c_str(),
					map[i].cfg.username.c_str(),
					map[i].cfg.email.c_str() };
			}
			break;
		}
		case Inactive: {
			fmt{ fmt_def, fc_none, "You need initialize repository\n" };
			break;
		}
		case notAuthorized: {
			fmt{ fmt_15ms, fc_none, "Type cmap set <YourUsername> <YourEmail> --user-config" };
			fmt{ fmt_def, fc_none, "  (You can't check logs without user data)\n" };
			break;
		}
		}
	});

	return cli.parse(argc, argv);
}