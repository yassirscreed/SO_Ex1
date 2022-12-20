#include "fs/operations.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define NUM_THREADS 4

uint8_t const file_contents[] = "AAA!";
char const target_path1[] = "/f1";
char const target_path2[] = "/f2";
char const target_path3[] = "/f3";
char const target_path4[] = "/f4";
char const link_path1[] = "/l1";

typedef struct
{
    char const *path;
    ssize_t result;
} thread_arg_t;

void *thread_func(void *arg)
{
    thread_arg_t *thread_arg = (thread_arg_t *)arg;
    char const *path = thread_arg->path;
    int f = tfs_open(path, 0);
    thread_arg->result = tfs_write(f, file_contents, sizeof(file_contents));
    tfs_close(f);
    return NULL;
}

void write_contents(char const *path)
{
    int f = tfs_open(path, 0);
    assert(f != -1);

    assert(tfs_write(f, file_contents, sizeof(file_contents)) ==
           sizeof(file_contents));

    assert(tfs_close(f) != -1);
}

void assert_contents_ok(char const *path)
{
    int f = tfs_open(path, 0);
    assert(f != -1);

    uint8_t buffer[sizeof(file_contents)];
    assert(tfs_read(f, buffer, sizeof(buffer)) == sizeof(buffer));
    assert(memcmp(buffer, file_contents, sizeof(buffer)) == 0);

    assert(tfs_close(f) != -1);
}

int main()
{
    // init TécnicoFS
    tfs_params params = tfs_default_params();
    params.max_inode_count = 4;
    params.max_block_count = 2;
    assert(tfs_init(&params) != -1);

    // create file with content
    {
        int f1 = tfs_open(target_path1, TFS_O_CREAT);
        assert(f1 != -1);
        assert(tfs_close(f1) != -1);
        write_contents(target_path1);
        assert_contents_ok(target_path1); // check that the file was created correctly
    }

    // create hard link
    assert(tfs_link(target_path1, link_path1) != -1);

    pthread_t threads[NUM_THREADS];
    thread_arg_t thread_args[NUM_THREADS] = {
        {target_path2, 0},
        {target_path3, 0},
        {link_path1, 0},
        {target_path4, 0},
    };

    // create threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, thread_func, &thread_args[i]);
    }

    // wait for threads to complete
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // check results
    assert(thread_args[0].result == -1); // not possible, as the maximum number of data blocks was reached
    assert(thread_args[1].result == -1); // not possible, as the maximum number of data blocks was reached
    assert(thread_args[2].result == -1); // not possible, as the file was unlinked
    assert(thread_args[3].result == sizeof(file_contents));

    // destroy TécnicoFS
    assert(tfs_destroy() != -1);

    printf("Successful test.\n");

    return 0;
}
