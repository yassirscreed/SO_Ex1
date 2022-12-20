#include "fs/operations.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char *path1 = "/f1";
    char *path2 = "/f2";
    char *str_from_ext_file = "This is a test file to be copied over.";

    /* Tests different scenarios where tfs_copy_from_external_fs is expected to
     * fail */

    assert(tfs_init(NULL) != -1);

    int f1 = tfs_open(path1, TFS_O_CREAT);
    assert(f1 != -1);
    assert(tfs_close(f1) != -1);

    int f2 = tfs_open(path2, TFS_O_CREAT);
    assert(f2 != -1);
    assert(tfs_copy_from_external_fs("tests/overwritten.txt", path2) != -1);
    assert(tfs_close(f2) != -1);

    // Scenario 1: source file does not exist
    assert(tfs_copy_from_external_fs("tests/unexistent", path1) == -1);

    // Scenario 2: destination file is a directory
    assert(tfs_copy_from_external_fs("tests/file_to_copy_over512.txt", "/") == -1);

    // Scenario ?: copy is over the maximum size ( 1024 ) will never happen because size is always 1024 (sou estupido)
    // assert(tfs_copy_from_external_fs("/home/yassirscreed/SO/SO-Ex1/tests/file_to_copy_over1024.txt", path1) == -1);

    // Scenario 3: copy is not overwriting an existing file
    assert(tfs_copy_from_external_fs("tests/file_to_copy_over512.txt", path2) != -1);
    // assert if the file was not overwritten
    int f3 = tfs_open(path2, TFS_O_CREAT);
    char buffer[600];
    ssize_t r = tfs_read(f3, buffer, sizeof(buffer) - 1);
    assert(r != strlen(str_from_ext_file));

    // TODO: add more failure scenarios

    printf("Successful test.\n");

    return 0;
}
