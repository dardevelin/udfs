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
    src/chunk.c
    src/hook_chmod.c
    src/hook_chown.c
    src/hook_getattr.c
    src/hook_mkdir.c
    src/hook_mknod.c
    src/hook_open.c
    src/hook_read.c
    src/hook_readdir.c
    src/hook_rename.c
    src/hook_rmdir.c
    src/hook_unlink.c
    src/hook_utimens.c
    src/hook_write.c
    src/main.c
    src/metadata.c
)

TARGET_LINK_LIBRARIES(
    udfs
    fuse
    sqlite3
)
