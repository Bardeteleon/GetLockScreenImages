cmake_minimum_required(VERSION 3.15)

project(LockScreenImagesProject)

add_subdirectory(libs)

set(imagemetadata_src 
    src/ImageMetadata.cpp 
    src/ImageMetadata.h)
add_library(imagemetadata_lib STATIC ${imagemetadata_src})
target_link_libraries(imagemetadata_lib PUBLIC jpegsize tinyexif)

set(LockscreenLocator_src 
    src/LockscreenLocator.cpp 
    src/LockscreenLocator.h)
add_library(LockscreenLocator_lib STATIC ${LockscreenLocator_src})
target_compile_options(LockscreenLocator_lib PUBLIC /std:c++latest) # needed for "filesystem", its first implemented in C++17?
target_compile_options(LockscreenLocator_lib PUBLIC -DUNICODE) # needed for conversion between all those char types.. TCHAR, wstring, string...

add_executable(GetLockScreenImages src/GetLockScreenImages.cpp)
target_link_libraries(GetLockScreenImages PUBLIC imagemetadata_lib LockscreenLocator_lib)
target_compile_options(GetLockScreenImages PUBLIC /std:c++latest) # needed for "filesystem", its first implemented in C++17?
target_compile_options(GetLockScreenImages PUBLIC -DUNICODE) # needed for conversion between all those char types.. TCHAR, wstring, string...