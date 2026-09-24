cmake_minimum_required(VERSION 3.16)
project(TimeConverter C CXX)

# Ищем все .c файлы внутри sofa_src/src
file(GLOB_RECURSE SOFA_SOURCES "sofa_src/src/*.c")

# Создаём статическую библиотеку из исходников SOFA
add_library(sofa STATIC ${SOFA_SOURCES})

# Указываем, где искать заголовочные файлы
target_include_directories(sofa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sofa_src/src)

# Наш исполняемый файл: добавляем все .cpp
add_executable(time_converter
    main.cpp
    Time.cpp
    TimeConverter.cpp
)

# Линкуем с библиотекой SOFA
target_link_libraries(time_converter PRIVATE sofa)