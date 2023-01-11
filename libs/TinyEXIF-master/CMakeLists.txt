set(sources 
    TinyEXIF.cpp 
    TinyEXIF.h
    EXIFStreamFile.h)

add_library(tinyexif STATIC ${sources})

target_include_directories(tinyexif PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(tinyexif PRIVATE tinyxml2)