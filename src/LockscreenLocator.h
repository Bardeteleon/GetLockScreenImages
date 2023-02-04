
#ifndef SRC_LOCKSCREENLOCATOR_H
#define SRC_LOCKSCREENLOCATOR_H

#include <vector>
#include "ImageMetadata.h"

class LockscreenLocator
{
  public:
    LockscreenLocator() = default;

    void Run(void);

    std::vector<ImageMetadata> GetLockscreenMetadata(void);

  private:

    void LocatePotentialLockscreens(void);
    void FilterRealLockscreens(void);

    std::wstring GetCurrentWindowsUserName(void);
    std::string ConvertWStringToString(const std::wstring& w_string);

    std::vector<ImageMetadata> potential_lockscreens_;
    
    const std::string user_name_placeholder_{"NameOfCurrentUserXYZ"};
    const std::string folder_name_source_template_{"C:/Users/" + user_name_placeholder_ + "/AppData/Local/Packages/Microsoft.Windows.ContentDeliveryManager_cw5n1h2txyewy/LocalState/Assets"};

};

#endif