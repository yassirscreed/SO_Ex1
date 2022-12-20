/*
This test will create three threads, each of which will create a file, create a hard link to the file, unlink the file,
and then check if the data in the hard link is still correct.
*/
#include "fs/operations.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

void *thread_func(void *arg)
{
    const char *file_path = (const char *)arg;

    // Create file
    int fd = tfs_open(file_path, TFS_O_CREAT);
    assert(fd != -1);

    const char write_contents[] = "Hello World!";

    // Write to file
    assert(tfs_write(fd, write_contents, sizeof(write_contents)));

    assert(tfs_close(fd) != -1);

    const char *link_path = "/l1";
    assert(tfs_link(file_path, link_path) != -1);

    // Unlink file
    assert(tfs_unlink(file_path) != -1);

    // Create new file with the same name
    fd = tfs_open(link_path, TFS_O_CREAT);
    assert(fd != -1);

    // Check if file still contains the correct data
    char read_contents[sizeof(write_contents)];
    assert(tfs_read(fd, read_contents, sizeof(read_contents)) != -1);
    assert(strcmp(read_contents, write_contents) == 0);

    return NULL;
}

int main()
{
    assert(tfs_init(NULL) != -1);

    pthread_t thread1, thread2, thread3;
    const char *file_path1 = "/f1";
    const char *file_path2 = "/f2";
    const char *file_path3 = "/f3";

    // Create and start the threads
    pthread_create(&thread1, NULL, thread_func, (void *)file_path1);
    pthread_create(&thread2, NULL, thread_func, (void *)file_path2);
    pthread_create(&thread3, NULL, thread_func, (void *)file_path3);

    // Wait for the threads to complete
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    printf("Successful test.\n");

    return 0;
}
