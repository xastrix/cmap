#pragma once

/* Path to the directory containing the repository files */
#define ENV_BASE_DIRECTORY             ".cmap"

/* Name of the file containing the list of ignored files */
#define ENV_IGNORE_LIST_FILENAME       ENV_BASE_DIRECTORY "ignore"

/* Regex pattern to disallow adding repository files */
#define ENV_BASE_DIRECTORY_PATTERN     R"(^\.(cmap|cmapignore)([\\/].*)?$)"

/* Filename of configuration file containing user information */
#define ENV_CONFIGURATION_FILENAME     ENV_BASE_DIRECTORY "config"

/* Storage directory for files within the base directory */
#define ENV_STORAGE_DIRECTORY          ENV_BASE_DIRECTORY "\\storage"

/* Filename with info of Temp/Tracking files */
#define ENV_FILES_FILENAME             ENV_BASE_DIRECTORY "\\files"

/* Filename with info of commit data */
#define ENV_COMMITMAP_FILENAME         ENV_BASE_DIRECTORY "\\map"

/* Length of the commit hash */
#define ENV_COMMIT_HASH_LENGTH         32

/* Set of characters used for generating random parts of the commit hash */
#define ENV_COMMIT_HASH_RANDOM_CHARS   "fdaceb0123456789"

/* Member name of temp files */
#define ENV_TEMP_FILES_MEMBER_NAME     "Temp"

/* Member name of tracking files */
#define ENV_TRACKING_FILES_MEMBER_NAME "Tracking"