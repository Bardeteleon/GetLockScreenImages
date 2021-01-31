
set(jpegsize_src 
    jpegsize.cpp 
    jpegsize.h)

add_library(jpegsize STATIC ${jpegsize_src})

target_include_directories(jpegsize PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})