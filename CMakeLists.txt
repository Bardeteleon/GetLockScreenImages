cmake_minimum_required(VERSION 3.15)

project(LockScreenImagesProject)

add_subdirectory(libs)

set(imagemetadata_src 
    src/ImageMetadata.cpp 
    src/ImageMetadata.h)

add_library(imagemetadata_lib STATIC ${imagemetadata_src})

target_link_libraries(imagemetadata_lib PUBLIC jpegsize tinyexif)

add_executable(GetLockScreenImages src/GetLockScreenImages.cpp)

target_link_libraries(GetLockScreenImages PUBLIC jpegsize tinyexif imagemetadata_lib)

target_compile_options(GetLockScreenImages PUBLIC /std:c++latest) # needed for "filesystem", its first implemented in C++17?
target_compile_options(GetLockScreenImages PUBLIC -DUNICODE) # needed for conversion between all those char types.. TCHAR, wstring, string...