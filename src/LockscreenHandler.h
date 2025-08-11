#ifndef SRC_LOCKSCREENHANDLER_H
#define SRC_LOCKSCREENHANDLER_H

#include <string>
#include <vector>
#include "ImageMetadata.h"

class LockscreenHandler
{
  public:
    LockscreenHandler() = default;

    void Init();
    void IntegrateNewLockscreens(const std::vector<ImageMetadata> lockscreens);

  private:
    int FindMaximumLockscreenIndex(const std::string& rel_path, const std::string& file_name_generic, const std::string& file_extension);
    void DetectCurrentWorkingDirectory();
    void TryCreateImageDirectories();
    void DetectExistingImages();

    bool init_success_{false};

    int max_index_landscape_{};
    int max_index_upright_{};

    std::string current_working_directory_{};

    const std::string folder_name_destination_{"LockScreens"};
    const std::string folder_name_destination_upright_{"Upright"};
    const std::string folder_name_destination_landscape_{"Landscape"};
    const std::string file_name_generic_{"Image_"};
    const std::string file_extension_{".jpg"};
};

#endif
