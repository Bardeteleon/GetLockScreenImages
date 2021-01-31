#include <stdio.h>
#include <Windows.h>
#include <winerror.h>
#include <iostream>
#include <fstream>
#include <filesystem> // settings for C++17:  Project > Properties > C/C++ > Language > C++ Language Standard
#include <sstream>
#include <regex>
#include "jpegsize.h"
#include <Lmcons.h>
#include <sstream>
#include "TinyEXIF.h"

namespace fs = std::filesystem;

std::string folder_name_source = "C:/Users/Konstantin Gerks/AppData/Local/Packages/Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy/LocalState/Assets";
//string folder_name_source = "C:/Users/Konstantin Gerks/Pictures/_Wallpapers/_RenameTool/InitData";

std::string folder_name_destination = "LockScreens";
std::string folder_name_destination_upright = "Upright";
std::string folder_name_destination_landscape = "Landscape";

std::string file_name_generic = "Image_";

std::string file_extension = ".jpg";

//vector<string> file_paths;

void ReplaceStringInPlace(std::string& subject, const std::string& search,	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

class EXIFStreamFile : public TinyEXIF::EXIFStream {
	private:
		std::ifstream file;
		std::vector<uint8_t> buffer;

	public:
		explicit EXIFStreamFile(const char* fileName)
			: file(fileName, std::ifstream::in | std::ifstream::binary) {
		}

		bool IsValid() const override {
			return file.is_open();
		}

		const uint8_t* GetBuffer(unsigned desiredLength) override {
			buffer.resize(desiredLength);
			if (!file.read((char*)buffer.data(), desiredLength))
				return NULL;
			return buffer.data();
		}

		bool SkipBuffer(unsigned desiredLength) override {
			return (bool)file.seekg(desiredLength, std::ios::cur);
		}
};

int find_maximum_image_index(std::string rel_path, std::string file_name_generic, std::string file_extension) {
	int max_counter = 0;
	int count_image_existing = 0;
	for (auto& p : fs::directory_iterator(rel_path))
	{
		std::string pattern_string = "(" + file_name_generic + "\\d+(?=" + file_extension + "$))";
		std::regex pat{ pattern_string };
		std::smatch matches;
		std::string path_string = p.path().string();
		// cout << path_string << "\n";

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

int main()
{
	std::cout << "Welcome!\n";
	std::cout << "This program delivers all Windows 10 lockscreen wallpapers it can find in the system.\n";
	std::cout << "These are the images you see when you log into your computer.\n";

	/* replace current user name in source path*/
	TCHAR username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);
	std::wstring username_w(&username[0]);
	std::string username_str(username_w.begin(), username_w.end());
	//cout << username_str;
	ReplaceStringInPlace(folder_name_source, "Konstantin Gerks", username_str);

	/* get path of .exe */
	TCHAR path[1040]; // MAX_PATH
	GetCurrentDirectory(sizeof(path), path);
	fs::path local_path = path;
	//local_path = local_path.remove_filename(); // not needed
	std::string local_path_string = local_path.string();
	//ReplaceStringInPlace(local_path_string, "\\", "/");

	/* LockScreens folder */
	if (CreateDirectoryA(std::string(local_path_string + "\\" + folder_name_destination).c_str(), NULL))
	{
		std::cout << " - Created a LockScreens folder next to the .exe\n";
		if (!CreateDirectoryA(std::string(local_path_string + "\\" + folder_name_destination + "\\" + folder_name_destination_landscape).c_str(), NULL)
			|| !CreateDirectoryA(std::string(local_path_string + "\\" + folder_name_destination + "\\" + folder_name_destination_upright).c_str(), NULL))
		{
			std::cout << " - Error: Not able to create subfolders.";
			return 0;
		}
	}
	else if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		std::cout << " - Detected an already existing LockScreens folder\n";
	}
	else
	{
		std::cout << " - Error: Not able to create the LockScreens folder.\n-> Termination\n";
		return 0;
	}

	int max_index_landscape = find_maximum_image_index(folder_name_destination + "\\" + folder_name_destination_landscape, file_name_generic, file_extension);
	int max_index_upright = find_maximum_image_index(folder_name_destination + "\\" + folder_name_destination_upright, file_name_generic, file_extension);

	/* Get all files in source folder */
	int counter_landscape = max_index_landscape + 1;
	int counter_upright = max_index_upright + 1;
	for (auto& p : fs::directory_iterator(folder_name_source))
	{
		/* read from files */
		//ifstream file(p.path());
		//char line[100];
		//file.getline(line, 100);
		//cout << line << "\n";

		/* store file paths in a list */
		//file_paths.push_back(p.path().string());

		//cout << "\n\nFile: " << p.path().filename() << "\n";

		/* source file */
		std::string source_path_string = p.path().string();
		ReplaceStringInPlace(source_path_string, "/", "\\");
		fs::path source_path = source_path_string;

		/* try to read image size
		   continue with next file if:
			- file not a jpeg
			- image size smaller 1000 pixel
		*/
		int image_width = 0;
		int image_height = 0;

		bool exif_abort = false;
		EXIFStreamFile img_stream(source_path_string.c_str());
		if (!img_stream.IsValid())
		{
			//cout << "EXIF: Can not open " << source_path_string << "\n";
			exif_abort = true;
		}
		else 
		{
			TinyEXIF::EXIFInfo img_exif(img_stream);
			if (!img_exif.Fields)
			{
				//cout << "EXIF: No EXIF or XMP metadata\n";
				exif_abort = true;
			}
			else
			{
				image_width = img_exif.ImageWidth;
				image_height = img_exif.ImageHeight;
				if (image_width < 1000 || image_height < 1000)
				{
					//cout << "EXIF: Image too small\n";
					exif_abort = true;
				}
			}
		}

		bool jpegsize_abort = false;
		FILE* p_file;
		fopen_s(&p_file, source_path_string.c_str(), "r");
		if (scanhead(p_file, &image_width, &image_height))
		{
			//cout << "Size: " << image_width << " - " << image_height << "\n";
			if (image_width <= 1000 || image_height <= 1000)
			{
				//cout << "jpegsize: Image too small\n";
				jpegsize_abort = true;
			}
		}
		else
		{
			//cout << "jpegsize: File not detected as jpeg\n";
			jpegsize_abort = true;
		}
		fclose(p_file);

		// file not detected as image (jpeg and exif data) or too small image size
		if (exif_abort && jpegsize_abort)
		{
			//cout << "File not detected as image (jpeg or exif data) or it is too small (<1000 pixels)\n";
			continue;
		}

		std::string folder_name_destination_type = "unset";
		int current_counter = 0;
		if (image_height > image_width) {
			folder_name_destination_type = folder_name_destination_upright;
			current_counter = counter_upright;
			counter_upright++;
		}
		else {
			folder_name_destination_type = folder_name_destination_landscape;
			current_counter = counter_landscape;
			counter_landscape++;
		}

		/* create desired path with file rename */
		std::ostringstream outStream;
		outStream << local_path_string << "\\" << folder_name_destination << "\\" << folder_name_destination_type << "\\" << file_name_generic << current_counter << file_extension;
		fs::path desired_path = outStream.str();

		/* copy file and add .jpg extension */
		try {
			if (fs::copy_file(source_path, desired_path))
			//if(fs::copy_file("Lock Screens\\test.txt", "Lock Screens\\test_copy2.txt"))
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