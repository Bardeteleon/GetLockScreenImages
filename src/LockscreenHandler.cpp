#include "LockscreenHandler.h"
#include <regex>
#include <iostream>
#include <filesystem>
#include <Windows.h>
#include <sstream>

void LockscreenHandler::Init()
{
    DetectCurrentWorkingDirectory();
    TryCreateImageDirectories();
    DetectExistingImages();
}

void LockscreenHandler::DetectCurrentWorkingDirectory()
{
	TCHAR path[1040]; // MAX_PATH
	GetCurrentDirectory(sizeof(path), path);
	std::filesystem::path local_path = path;
	current_working_directory_ = local_path.string();
}

void LockscreenHandler::TryCreateImageDirectories()
{
	if (CreateDirectoryA(std::string(current_working_directory_ + "\\" + folder_name_destination_).c_str(), NULL))
	{
		std::cout << " - Created a LockScreens folder next to the .exe\n";
		if (!CreateDirectoryA(std::string(current_working_directory_ + "\\" + folder_name_destination_ + "\\" + folder_name_destination_landscape_).c_str(), NULL)
			|| !CreateDirectoryA(std::string(current_working_directory_ + "\\" + folder_name_destination_ + "\\" + folder_name_destination_upright_).c_str(), NULL))
		{
			std::cout << " - Error: Not able to create subfolders.";
			init_success_ = false;
		}
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		std::cout << " - Detected an already existing LockScreens folder\n";
		init_success_ = true;
	}
	else
	{
		std::cout << " - Error: Not able to create the LockScreens folder.\n-> Termination\n";
		init_success_ = false;
	}
}

void LockscreenHandler::DetectExistingImages()
{
	max_index_landscape_ = FindMaximumLockscreenIndex(folder_name_destination_ + "\\" + folder_name_destination_landscape_, file_name_generic_, file_extension_);
	max_index_upright_ = FindMaximumLockscreenIndex(folder_name_destination_ + "\\" + folder_name_destination_upright_, file_name_generic_, file_extension_);
}

int LockscreenHandler::FindMaximumLockscreenIndex(const std::string& rel_path, const std::string& file_name_generic, const std::string& file_extension) {
	int max_counter = 0;
	int count_image_existing = 0;
	for (auto& p : std::filesystem::directory_iterator(rel_path))
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

void LockscreenHandler::IntegrateNewLockscreens(const std::vector<ImageMetadata> lockscreens)
{
	/* Get all files in source folder */
	int counter_landscape = max_index_landscape_ + 1;
	int counter_upright = max_index_upright_ + 1;
	for (const auto& image : lockscreens)
	{
		std::string folder_name_destination_type = "unset";
		int current_counter = 0;
		if (image.IsLandscape()){
			folder_name_destination_type = folder_name_destination_landscape_;
			current_counter = counter_landscape;
			counter_landscape++;
		}
		else {
			folder_name_destination_type = folder_name_destination_upright_;
			current_counter = counter_upright;
			counter_upright++;
		}

		/* create desired path with file rename */
		std::ostringstream outStream;
		outStream << current_working_directory_ << "\\" << folder_name_destination_ << "\\" << folder_name_destination_type << "\\" << file_name_generic_ << current_counter << file_extension_;
		std::filesystem::path desired_path = outStream.str();

		/* copy file and add .jpg extension */
		std::filesystem::path source_path = image.GetPath();
		try {
			if (std::filesystem::copy_file(source_path, desired_path))
			{
				//cout << "Image copied: " << desired_path.filename() << "\n";
			}
		}
		catch (std::filesystem::filesystem_error & e) {
			std::cout << " - Could not copy: " << e.what() << '\n';
			std::cout << "\n!!! Check your Security Programm, it may blocks this program to create files !!!\n\n";
			std::cout << "Press ENTER to close the window ...";
			std::cin.get();
		}
	}

	std::cout << " - Copied " << (counter_landscape - max_index_landscape_ - 1) << " images to each folder.\n";
	std::cout << "\nHave a look! :)\n\n";
}
