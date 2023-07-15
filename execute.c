/* expression.c */

#include "search.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * Executes the -print or -exec expressions (or both) on the specified path.
 * @param   path        Path to a file or directory
 * @param   settings    Settings structure.
 * @return  Whether or not the execution was successful.
 */
int	    execute(const char *path, const Settings *settings) {
    pid_t pid;
    int status;
    int p[2], to_parent, from_child;
    char buffer[1024];
    ssize_t nread;

    char **args = (char**)malloc(sizeof(char*) * settings->exec_argc+1);//check if malloc was successful
    for(int i = 0; i < settings->exec_argc; i++){
        if(strcmp(settings->exec_argv[i], "<path>") == 0)
        { 
            args[i] = (char*)malloc(sizeof(char) * (strlen(path)+1));
            if(strlcpy(args[i], path, strlen(path) + 1) == NULL)
            {
                fprintf(stderr, "Failed to copy path to args.");
                exit(1);   
            } 
        }
        else
        {
            args[i] = (char*)malloc(strlen(settings->exec_argv[i])+1);
            strlcpy(args[i], settings->exec_argv[i], strlen(settings->exec_argv[i])+1);
        }
    }
   
    // args[settings->exec_argc] = (char*)malloc(sizeof(char) * (strlen(path)+1));
    // if(strlcpy(args[settings->exec_argc], path, strlen(path) + 1) == NULL)
    // {
    //     fprintf(stderr, "Failed to copy path to argv.");
    //     exit(1);   
    // }

    settings->exec_argv[settings->exec_argc+1] = NULL;

    pipe(p);
    from_child = p[0];
    to_parent = p[1];
    pid = fork();
    if(pid == 0)
    {
        close(from_child);
        dup2(STDOUT_FILENO, to_parent);
        dup2(STDERR_FILENO, to_parent);
        //this is child process
        if(execvp(args[0], args) < 0)
        {

            fprintf(stderr, "Failed to execute the given program %s", settings->exec_argv[0]);
            exit(1);
        }
        return 0;
    }
    else if (pid < 0)
    {
        fprintf(stderr, "Failed to fork the process");
        exit(1);
    }
    
    else
    {
        close(to_parent);
        while ((nread=read(from_child, &buffer[0], sizeof(buffer))) > 0) 
        {
            write(STDOUT_FILENO, &buffer[0], nread);
        }
        buffer[0] = '\n';
        write(STDOUT_FILENO, &buffer[0], 1);
        close(from_child);
        wait(&status);
        if(WIFEXITED(status) != 0)
        {
            printf("Child process complete with status: %d\n", WEXITSTATUS(status));
        }
        else{
            printf("Child exited abnormally with status: %d\n", WEXITSTATUS(status));
        }
        
    }

    
    return 0;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
