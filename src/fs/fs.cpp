#include "fs.h"

#include "../config.h"
#include "../fmt/fmt.h"

#include <fstream>

fs_obj fs::exists(const std::string& path)
{
	struct stat   s;
	struct fs_obj o;

	o._status = existNone;

	if (stat(path.c_str(), &s) == 0)
	{
		if (s.st_mode & S_IFDIR) {
			o._status = existDirectory;
		}
		else if (s.st_mode & S_IFREG) {
			o._status = existObject;
		}
	}

	return o;
}

long fs::get_file_size(const std::string& filename)
{
	FILE* f = fopen(filename.c_str(), "rb");
	if (f == NULL) {
		return -1;
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);

	fclose(f);
	return size;
}

bool fs::make_directory(const std::string& dirname, dir_attrib attributes)
{
	if (!std::filesystem::create_directory(dirname))
		return false;

	if (attributes != 0) {
		DWORD attrib = GetFileAttributesA(dirname.c_str());
		attrib |= attributes;
		if (!SetFileAttributesA(dirname.c_str(), attrib)) {}
	}

	return true;
}

bool fs::make_file(const std::string& filename, const std::string& content, int flags)
{
	std::ofstream f{ filename, flags };

	if (!f.is_open())
		return false;

	f << content;
	f.close();

	return true;
}

void fs::delete_objects(const std::vector<std::string>& objects)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (!exists(objects[i]).as(existNone))
		{
			auto c = std::filesystem::remove_all(objects[i]);
		}
	}
}

std::string fs::get_file_content(const std::string& filename)
{
	std::ifstream f{ filename, std::ios::binary };

	if (!f.is_open())
		return "";

	return std::string{ std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>() };
}

void fs::get_directory_files(const std::string& dirname, char** files, int* num, find_mode mode)
{
	WIN32_FIND_DATA data;
	HANDLE h;

	char path[MAX_PATH];

	if (dirname == ".")
		sprintf(path, "*.*");
	else
		sprintf(path, "%s\\*.*", dirname.c_str());

	h = FindFirstFileA(path, &data);

	do {
		if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
			continue;
		
		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && mode == fmFiles)
			continue;

		char path[MAX_PATH];

		if (dirname == ".")
			sprintf(path, "%s", data.cFileName);
		else
			sprintf(path, "%s\\%s", dirname.c_str(), data.cFileName);

		if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (mode == fmRecursive) {
				get_directory_files(path, files, num, fmRecursive);
			}
			continue;
		}

		if (*num < MAX_FILES) {
			files[*num] = new char[strlen(path) + 1];
			if (files[*num] != NULL) {
				strcpy_s(files[*num], strlen(path) + 1, path);
				(*num)++;
			}
		}

	} while (FindNextFileA(h, &data) != 0);

	FindClose(h);
}

void fs::find_similar_files(const std::string& keyword, const std::string& dirname, char* similars[MAX_SIMILARS], int* num)
{
	char* files[MAX_FILES];
	int   file_num = 0;

	get_directory_files(dirname, files, &file_num, fmFiles);

	for (int i = 0; i < file_num; i++) {
		if (strncmp(files[i], keyword.c_str(), 1) == 0) {
			if (*num < MAX_SIMILARS) {
				similars[*num] = files[i];
				(*num)++;
			}
			else {
				break;
			}
		}
	}
}

bool fs::find_files_in_directories(const std::string& dirname, const std::string& keyword)
{
	bool ret = false;

	if (!exists(dirname).as(existDirectory))
		return ret;

	for (const auto& entry : std::filesystem::directory_iterator(dirname))
	{
		if (entry.path().filename().string() == keyword)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void fs::copy(const std::string& source, const std::string& destination, const std::filesystem::copy_options options)
{
	try {
		std::filesystem::create_directories(std::filesystem::path(destination).parent_path());
		std::filesystem::copy(source, destination, options);
	}
	catch (const std::filesystem::filesystem_error& e) {
		fmt{ fc_red, "%s\n", e.what() };
	}
	catch (const std::exception& e) {
		fmt{ fc_red, "%s\n", e.what() };
	}
}