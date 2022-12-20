/*
This test creates three threads: one that writes to the symbolic link link_path1, another that reads from the target file target_path1 and another that deletes and recreates the symbolic link.
Both threads perform these operations concurrently for 100 iterations each.
*/

#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t const file_contents[] = "AAA!";
char const target_path1[] = "/f1";
char const link_path1[] = "/l1";

void *write_thread()
{
    for (int i = 0; i < 100; i++)
    {
        int f = tfs_open(link_path1, 0);
        assert(f != -1);

        assert(tfs_write(f, file_contents, sizeof(file_contents)) == sizeof(file_contents));

        assert(tfs_close(f) != -1);
    }
    return NULL;
}

void *read_thread()
{
    for (int i = 0; i < 100; i++)
    {
        int f = tfs_open(target_path1, 0);
        assert(f != -1);

        uint8_t buffer[sizeof(file_contents)];
        assert(tfs_read(f, buffer, sizeof(buffer)) == sizeof(buffer));
        assert(memcmp(buffer, file_contents, sizeof(buffer)) == 0);

        assert(tfs_close(f) != -1);
    }
    return NULL;
}

void *delete_thread()
{
    for (int i = 0; i < 50; i++)
    {
        assert(tfs_unlink(link_path1) != -1);
        assert(tfs_sym_link(target_path1, link_path1) != -1);
    }
    return NULL;
}

int main()
{
    assert(tfs_init(NULL) != -1);

    // Create target file
    int f = tfs_open(target_path1, TFS_O_CREAT);
    assert(f != -1);
    assert(tfs_close(f) != -1);

    // Create symbolic link
    assert(tfs_sym_link(target_path1, link_path1) != -1);

    // Create three threads: one that writes to the symbolic link, another that reads from the target file, and another that deletes and recreates the symbolic link
    pthread_t write_tid;
    pthread_t read_tid;
    pthread_t delete_tid;
    assert(pthread_create(&write_tid, NULL, write_thread, NULL) == 0);
    assert(pthread_create(&read_tid, NULL, read_thread, NULL) == 0);
    assert(pthread_create(&delete_tid, NULL, delete_thread, NULL) == 0);

    // Wait for the threads to finish
    assert(pthread_join(write_tid, NULL) == 0);
    assert(pthread_join(read_tid, NULL) == 0);
    assert(pthread_join(delete_tid, NULL) == 0);

    assert(tfs_destroy() != -1);

    printf("Successful test.\n");

    return 0;
}
