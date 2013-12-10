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
    src/hook_getattr.c
    src/hook_open.c
    src/hook_read.c
    src/hook_readdir.c
    src/main.c
    src/metadata.c
)

TARGET_LINK_LIBRARIES(
    udfs
    fuse
    event
    sqlite3
)
