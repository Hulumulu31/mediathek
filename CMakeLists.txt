cmake_minimum_required(VERSION 3.16)
project(mediathek LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)

find_path(LIBPQXX_INCLUDE pqxx/pqxx)
find_library(LIBPQXX_LIB NAMES pqxx libpqxx)

if(NOT LIBPQXX_INCLUDE OR NOT LIBPQXX_LIB)
    message(FATAL_ERROR "libpqxx not found. Install mingw-w64-x86_64-libpqxx (MSYS2).")
endif()

add_executable(mediathek
    src/main.cpp
    src/repository/database.cpp
    src/repository/track_repository.cpp
    src/repository/playlist_repository.cpp
    src/service/media_service.cpp
    src/cli/menu.cpp
)

target_include_directories(mediathek PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LIBPQXX_INCLUDE}
)

target_link_libraries(mediathek PRIVATE
    ${LIBPQXX_LIB}
    PostgreSQL::PostgreSQL
)
