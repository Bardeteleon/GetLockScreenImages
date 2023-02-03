#include <stdio.h>
#include <Windows.h>
#include <winerror.h>
#include <iostream>
#include <filesystem> // settings for C++17:  Project > Properties > C/C++ > Language > C++ Language Standard
#include <sstream>
#include <regex>
#include <Lmcons.h>
#include <sstream>
#include "EXIFStreamFile.h"
#include "jpegsize.h"
#include "ImageMetadata.h"

namespace fs = std::filesystem;

const std::string user_name_placeholder = "NameOfCurrentUserXYZ";
const std::string folder_name_source_template = "C:/Users/" + user_name_placeholder + "/AppData/Local/Packages/Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy/LocalState/Assets";
const std::string folder_name_destination = "LockScreens";
const std::string folder_name_destination_upright = "Upright";
const std::string folder_name_destination_landscape = "Landscape";
const std::string file_name_generic = "Image_";
const std::string file_extension = ".jpg";

std::string ReplaceStringInPlace(const std::string& subject, const std::string& search, const std::string& replace) {
	size_t pos = 0;
	std::string result = subject;
	while ((pos = result.find(search, pos)) != std::string::npos) {
		result.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return result;
}

int find_maximum_image_index(const std::string& rel_path, const std::string& file_name_generic, const std::string& file_extension) {
	int max_counter = 0;
	int count_image_existing = 0;
	for (auto& p : fs::directory_iterator(rel_path))
	{
		std::string pattern_string = "(" + file_name_generic + "\\d+(?=" + file_extension + "$))";
		std::regex pat{ pattern_string };
		std::smatch matches;
		std::string path_string = p.path().string();

		/* check file path format */
		if (std::regex_search(path_string, matches, pat))
		{
			std::string file_number_string = matches[0].str().replace(0, file_name_generic.length(), "");
			int file_number_int = std::stoi(file_number_string);
			if (file_number_int > max_counter)
			{
				max_counter = file_number_int;
			}
			//cout << "Results: " << file_number_int << "\n";
		}
		else
		{
			//cout << " - File does not match the expected sturcture: " << path_string << "\n";
			continue;
		}
		count_image_existing++;
	}
	if (max_counter > 0)
	{
		std::cout << " - Found " << count_image_existing << " images in the folder '" << rel_path << "' with maximum number " << max_counter << "\n";
	}
	return max_counter;
}

std::wstring GetCurrentWindowsUserName(void)
{
	TCHAR username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);
	std::wstring username_w(&username[0]);

	return username_w;
}

std::string ConvertWStringToString(const std::wstring& w_string)
{
	return std::string(w_string.begin(), w_string.end());
}

std::string GetCurrentWorkingDirectory(void)
{
	TCHAR path[1040]; // MAX_PATH
	GetCurrentDirectory(sizeof(path), path);
	fs::path local_path = path;
	return local_path.string();
}

bool TryCreateImageDirectories(const std::string& destination)
{
	bool success;

	if (CreateDirectoryA(std::string(destination + "\\" + folder_name_destination).c_str(), NULL))
	{
		std::cout << " - Created a LockScreens folder next to the .exe\n";
		if (!CreateDirectoryA(std::string(destination + "\\" + folder_name_destination + "\\" + folder_name_destination_landscape).c_str(), NULL)
			|| !CreateDirectoryA(std::string(destination + "\\" + folder_name_destination + "\\" + folder_name_destination_upright).c_str(), NULL))
		{
			std::cout << " - Error: Not able to create subfolders.";
			success = false;
		}
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		std::cout << " - Detected an already existing LockScreens folder\n";
		success = true;
	}
	else
	{
		std::cout << " - Error: Not able to create the LockScreens folder.\n-> Termination\n";
		success = false;
	}

	return success;
}

int main()
{
	std::cout << "Welcome!\n";
	std::cout << "This program delivers all Windows 10 lockscreen wallpapers it can find in the system.\n";
	std::cout << "These are the images you see when you log into your computer.\n";

	/* replace current user name in source path*/
	std::wstring username_w{ GetCurrentWindowsUserName() };
	std::string username_str{ ConvertWStringToString(username_w) };
	std::string folder_name_source{ ReplaceStringInPlace(folder_name_source_template, user_name_placeholder, username_str) };

	/* get path of .exe */
	std::string current_working_dir = GetCurrentWorkingDirectory();

	/* LockScreens folder */
	bool success = TryCreateImageDirectories(current_working_dir);
	if (!success)
		return 1;

	std::vector<ImageMetadata> images;
	for (const auto& directory_entry : fs::directory_iterator(folder_name_source))
	{
		std::string source_path_string = directory_entry.path().string();
		source_path_string = ReplaceStringInPlace(source_path_string, "/", "\\");

		/* try to read image size
		   continue with next file if:
			- file not a jpeg
			- image size smaller 1000 pixel
		*/
		images.emplace_back(source_path_string);
	}

	int max_index_landscape = find_maximum_image_index(folder_name_destination + "\\" + folder_name_destination_landscape, file_name_generic, file_extension);
	int max_index_upright = find_maximum_image_index(folder_name_destination + "\\" + folder_name_destination_upright, file_name_generic, file_extension);

	/* Get all files in source folder */
	int counter_landscape = max_index_landscape + 1;
	int counter_upright = max_index_upright + 1;
	for (const auto& image : images)
	{
		// file not detected as image (jpeg and exif data) or too small image size
		if (!image.IsValid())
		{
			//cout << "File not detected as image (jpeg or exif data) or it is too small (<1000 pixels)\n";
			continue;
		}

		std::string folder_name_destination_type = "unset";
		int current_counter = 0;
		if (image.IsLandscape()){
			folder_name_destination_type = folder_name_destination_landscape;
			current_counter = counter_landscape;
			counter_landscape++;
		}
		else {
			folder_name_destination_type = folder_name_destination_upright;
			current_counter = counter_upright;
			counter_upright++;
		}

		/* create desired path with file rename */
		std::ostringstream outStream;
		outStream << current_working_dir << "\\" << folder_name_destination << "\\" << folder_name_destination_type << "\\" << file_name_generic << current_counter << file_extension;
		fs::path desired_path = outStream.str();

		/* copy file and add .jpg extension */
		fs::path source_path = image.GetPath();
		try {
			if (fs::copy_file(source_path, desired_path))
			{
				//cout << "Image copied: " << desired_path.filename() << "\n";
			}
		}
		catch (fs::filesystem_error & e) {
			std::cout << " - Could not copy: " << e.what() << '\n';
			std::cout << "\n!!! Check your Security Programm, it may blocks this program to create files !!!\n\n";
			std::cout << "Press ENTER to close the window ...";
			std::cin.get();
			return 0 ;
		}
	}

	std::cout << " - Copied " << (counter_landscape - max_index_landscape - 1) << " images to each folder.\n";
	std::cout << "\nHave a look! :)\n\n";

	std::cout << "Press ENTER to close the window ...";
	std::cin.get();

	return 0;
}