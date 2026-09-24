FILE(GLOB_RECURSE file/*.c)
add_library(sofa ${file})

target_include_directories(sofa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


PROJECT(progect)
add_executable(${PROJECT_NAME} main.cpp)

file(GLOB_RECURSE files src/*c)
add_library(sofa ${files})

include_directoties(sofa PUBLIC src)
message("SOFA")

