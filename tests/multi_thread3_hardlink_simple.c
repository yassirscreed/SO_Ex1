/*
Each thread executes the test_link() function, which performs the same operations as the original code to test tfs_link().
After all the threads have been created, the main thread waits for them to complete using the pthread_join() function.
This ensures that all the threads have finished executing before tfs_destroy() is called.
*/

#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define NUM_THREADS 4

uint8_t const file_contents[] = "AAA!";
char const target_path1[] = "/f1";
char const link_path1[] = "/l1";
char const target_path2[] = "/f2";
char const link_path2[] = "/l2";

void assert_contents_ok(char const *path)
{
    int f = tfs_open(path, 0);
    assert(f != -1);

    uint8_t buffer[sizeof(file_contents)];
    assert(tfs_read(f, buffer, sizeof(buffer)) == sizeof(buffer));
    assert(memcmp(buffer, file_contents, sizeof(buffer)) == 0);

    assert(tfs_close(f) != -1);
}

void assert_empty_file(char const *path)
{
    int f = tfs_open(path, 0);
    assert(f != -1);

    uint8_t buffer[sizeof(file_contents)];
    assert(tfs_read(f, buffer, sizeof(buffer)) == 0);

    assert(tfs_close(f) != -1);
}

void write_contents(char const *path)
{
    int f = tfs_open(path, 0);
    assert(f != -1);

    assert(tfs_write(f, file_contents, sizeof(file_contents)) ==
           sizeof(file_contents));

    assert(tfs_close(f) != -1);
}

void *test_link()
{
    // Write to link and read original file
    {
        int f = tfs_open(target_path1, TFS_O_CREAT);
        assert(f != -1);
        assert(tfs_close(f) != -1);

        assert_empty_file(target_path1); // Check that file is empty
    }

    assert(tfs_link(target_path1, link_path1) != -1);
    assert_empty_file(link_path1);

    write_contents(link_path1);
    assert_contents_ok(target_path1);

    // Write to original file and read through link
    {
        int f = tfs_open(target_path2, TFS_O_CREAT);
        assert(f != -1);
        assert(tfs_close(f) != -1);

        write_contents(target_path2);

        assert_contents_ok(target_path2);
    }

    assert(tfs_link(target_path2, link_path2) != -1);
    assert_contents_ok(link_path2);

    return NULL;
}

int main()
{
    assert(tfs_init(NULL) != -1);

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
    {
        int res = pthread_create(&threads[i], NULL, test_link, NULL);
        assert(res == 0);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        int res = pthread_join(threads[i], NULL);
        assert(res == 0);
    }

    assert(tfs_destroy() != -1);

    printf("Successful test.\n");

    return 0;
}
