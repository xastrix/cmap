#include "cli/cli.h"
#include "repo/repo.h"
#include "utils/utils.h"

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
			std::cout << "Type cmap set <YourUsername> <YourEmail> --user-config" << std::endl;
			std::cout << "  (You can't initialize repository without user data)" << std::endl;
			return;
		}

		if (repo::util::setup_base_files() == alreadyExists)
			repo.status = Active;

		std::cout << "Repository successful " <<
			(repo.status == Inactive ? "initialized" : "reinitialized") << std::endl;
		std::cout << "  (" << repo::util::get_repo_directory() << ")" << std::endl;
		
		std::cout << std::endl;

		std::cout << "Some info for newbies" << std::endl;

		std::cout << std::endl;

		std::cout << "Add files to your repository using \"cmap add <Filename>\"" << std::endl;
		std::cout << "  Or you can check the status of your files with \"cmap status\"" << std::endl;

		std::cout << std::endl;

		std::cout << "After these steps, make a commit with \"cmap commit <YourMessage>\"" << std::endl;
		std::cout << "  You can view commits along with their hashes, timestamps, and committers using \"cmap log\"." << std::endl;
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
			std::cout << "You need initialize repository" << std::endl;
			break;
		}
		case notAuthorized: {
			std::cout << "Type cmap set <YourUsername> <YourEmail> --user-config" << std::endl;
			std::cout << "  (You can't add files without user data)" << std::endl;
			break;
		}
		}
	});

	cli.add("commit", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			if (repo::util::get_files(tempFiles).empty()) {
				std::cout << "Add files to the staging area before committing" << std::endl;
				break;
			}

			if (ac == 1)
				repo::set_commit(cfg, args[1]);
			break;
		}
		case Inactive: {
			std::cout << "You need initialize repository" << std::endl;
			break;
		}
		case notAuthorized: {
			std::cout << "Type cmap set <YourUsername> <YourEmail> --user-config" << std::endl;
			std::cout << "  (You can't committing without user data)" << std::endl;
			break;
		}
		}
	});

	cli.add("status", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			repo::log(repoStatus);
			break;
		}
		case Inactive: {
			std::cout << "You need initialize repository" << std::endl;
			break;
		}
		case notAuthorized: {
			std::cout << "Type cmap set <YourUsername> <YourEmail> --user-config" << std::endl;
			std::cout << "  (You can't check status without user data)" << std::endl;
			break;
		}
		}
	});

	cli.add("log", [&](int ac, arguments_t args) {
		switch (repo.status) {
		case Active: {
			repo::log(repoLog);
			break;
		}
		case Inactive: {
			std::cout << "You need initialize repository" << std::endl;
			break;
		}
		case notAuthorized: {
			std::cout << "Type cmap set <YourUsername> <YourEmail> --user-config" << std::endl;
			std::cout << "  (You can't check logs without user data)" << std::endl;
			break;
		}
		}
	});

	return cli.parse(argc, argv);
}