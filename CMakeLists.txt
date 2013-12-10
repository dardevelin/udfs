CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(udfs)

ADD_DEFINITIONS(
    -D_FILE_OFFSET_BITS=64
    -D_GNU_SOURCE
    -std=c99
    -pedantic
)

INCLUDE_DIRECTORIES(
    include
)

ADD_EXECUTABLE(
    udfs
    src/main.c
)

TARGET_LINK_LIBRARIES(
    udfs
    fuse
    event
    sqlite3
)
