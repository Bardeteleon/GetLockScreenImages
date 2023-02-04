
#include <Windows.h>
#include <Lmcons.h>
#include <filesystem>
#include <regex>

#include "LockscreenLocator.h"

void LockscreenLocator::Run(void)
{
    /* replace current user name in source path*/
	std::wstring username_w{ GetCurrentWindowsUserName() };
	std::string username_str{ ConvertWStringToString(username_w) };
	std::string folder_name_source{ std::regex_replace(folder_name_source_template_, std::regex(user_name_placeholder_), username_str) };

	for (const auto& directory_entry : std::filesystem::directory_iterator(folder_name_source))
	{
		std::string source_path_string = directory_entry.path().string();
		source_path_string = std::regex_replace(source_path_string, std::regex("/"), "\\");

		/* try to read image size
		   continue with next file if:
			- file not a jpeg
			- image size smaller 1000 pixel
		*/
		potential_lockscreens_.emplace_back(source_path_string);
	}
}

std::vector<ImageMetadata> LockscreenLocator::GetLockscreenMetadata(void)
{
    return potential_lockscreens_;
}

std::wstring LockscreenLocator::GetCurrentWindowsUserName(void)
{
	TCHAR username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);
	std::wstring username_w(&username[0]);

	return username_w;
}

std::string LockscreenLocator::ConvertWStringToString(const std::wstring& w_string)
{
	return std::string(w_string.begin(), w_string.end());
}