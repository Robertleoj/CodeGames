#ifndef POPEN2_jfdkljfd09fdjfodfjd
#define POPEN2_jfdkljfd09fdjfodfjd

#include<stdio.h>
#include<string>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define READ 0
#define WRITE 1

pid_t popen2(const char *command, int *infp, int *outfp) {

    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);

        execl("/bin/sh", "sh", "-c", command, NULL);
        perror("execl");
        exit(1);
    }

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];

    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];

    return pid;
}

#endif