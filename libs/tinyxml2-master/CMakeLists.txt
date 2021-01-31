set(sources 
    tinyxml2.cpp 
    tinyxml2.h)

add_library(tinyxml2 STATIC ${sources})

target_include_directories(tinyxml2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})