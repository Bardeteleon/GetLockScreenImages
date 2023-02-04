#ifndef SRC_LOCKSCREENHANDLER_H
#define SRC_LOCKSCREENHANDLER_H

#include <string>
#include <vector>
#include "ImageMetadata.h"

class LockscreenHandler
{
  public:
    LockscreenHandler(void) = default;

    void Init(void);
    void IntegrateNewLockscreens(const std::vector<ImageMetadata> lockscreens);

  private:
    int FindMaximumLockscreenIndex(const std::string& rel_path, const std::string& file_name_generic, const std::string& file_extension);
    void DetectCurrentWorkingDirectory(void);
    void TryCreateImageDirectories(void);
    void DetectExistingImages(void);

    bool init_success_{false};

    int max_index_landscape_{};
    int max_index_upright_{};

    std::string current_working_directory_{};

    const std::string folder_name_destination = "LockScreens";
    const std::string folder_name_destination_upright = "Upright";
    const std::string folder_name_destination_landscape = "Landscape";
    const std::string file_name_generic = "Image_";
    const std::string file_extension = ".jpg";
};

#endif