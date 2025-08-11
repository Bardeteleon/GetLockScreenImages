cmake_minimum_required(VERSION 3.15)

project(LockScreenImagesProject)

add_compile_options("$<$<CONFIG:Debug>:/MTd>")
add_compile_options("$<$<CONFIG:Release>:/MT>")

add_subdirectory(libs)

add_subdirectory(src)

add_executable(GetLockScreenImages src/GetLockScreenImages.cpp)
target_link_libraries(GetLockScreenImages PUBLIC imagemetadata_lib LockscreenLocator_lib LockscreenHandler_lib)
