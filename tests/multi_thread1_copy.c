/*
This test will create multiple threads that each perform the same sequence of operations as the main thread.
It will then wait for all of the threads to finish before printing a success message.
This will allow you to test whether tfs_copy_from_external() works correctly with multiple threads accessing it simultaneously.
*/

#include <pthread.h>
#include "fs/operations.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define NUM_THREADS 4

char *str_ext_file =
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! BBB! "
    "BBB! BBB! BBB! BBB! BBB! ";

void *thread_func()
{
    char *path_copied_file = "/f1";
    char *path_src = "/home/yassirscreed/SO/SO-Ex1/tests/file_to_copy_over512.txt";
    char buffer[600];

    int f;
    ssize_t r;

    f = tfs_copy_from_external_fs(path_src, path_copied_file);
    assert(f != -1);

    f = tfs_open(path_copied_file, TFS_O_CREAT);
    assert(f != -1);

    r = tfs_read(f, buffer, sizeof(buffer) - 1);
    assert(r == strlen(str_ext_file));
    assert(!memcmp(buffer, str_ext_file, strlen(str_ext_file)));

    return NULL;
}

int main()
{

    assert(tfs_init(NULL) != -1);

    pthread_t threads[NUM_THREADS];
    int i;
    // Create NUM_THREADS threads
    for (i = 0; i < NUM_THREADS; i++)
    {
        assert(pthread_create(&threads[i], NULL, thread_func, NULL) == 0);
    }

    // Wait for all threads to finish
    for (i = 0; i < NUM_THREADS; i++)
    {
        assert(pthread_join(threads[i], NULL) == 0);
    }

    printf("Successful test.\n");

    return 0;
}
