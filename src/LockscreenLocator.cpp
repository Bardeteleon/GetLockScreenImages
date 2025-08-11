
#include <Windows.h>
#include <Lmcons.h>
#include <algorithm>
#include <filesystem>
#include <regex>

#include "LockscreenLocator.h"

void LockscreenLocator::Run()
{
    LocatePotentialLockscreens();
    FilterRealLockscreens();
}

void LockscreenLocator::LocatePotentialLockscreens()
{
	std::wstring username_w{ GetCurrentWindowsUserName() };
	std::string username_str{ ConvertWStringToString(username_w) };
	std::string folder_name_source{ std::regex_replace(folder_name_source_template_, std::regex(user_name_placeholder_), username_str) };

	for (const auto& directory_entry : std::filesystem::directory_iterator(folder_name_source))
	{
		std::string source_path_string = directory_entry.path().string();
		source_path_string = std::regex_replace(source_path_string, std::regex("/"), "\\");

		potential_lockscreens_.emplace_back(source_path_string);
	}
}

void LockscreenLocator::FilterRealLockscreens()
{
    potential_lockscreens_.erase(std::remove_if(potential_lockscreens_.begin(), potential_lockscreens_.end(), [](const ImageMetadata& image){
        return !image.IsValid() || image.GetHeight() < 1000 || image.GetWidth() < 1000;
    }), potential_lockscreens_.end());
}

std::vector<ImageMetadata> LockscreenLocator::GetLockscreenMetadata()
{
    return potential_lockscreens_;
}

std::wstring LockscreenLocator::GetCurrentWindowsUserName()
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
