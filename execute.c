/* expression.c */

#include "search.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

/**
 * Executes the -print or -exec expressions (or both) on the specified path.
 * @param   path        Path to a file or directory
 * @param   settings    Settings structure.
 * @return  Whether or not the execution was successful.
 */
int	    execute(const char *path, const Settings *settings) {
    pid_t pid;
    int p[2], to_parent, from_child;
    char buffer[1024];
    ssize_t nread;
    int argc = settings->exec_argc;

    /*
    Be aware when using pipes, this is actually two processes. 
    Reference this SO page: https://stackoverflow.com/questions/18603153/linux-execvp-ls-cannot-access-no-such-file-or-directory
    */
    char **args = (char**)malloc(sizeof(char*) * (argc+1));//check if malloc was successful
    for(int i = 0; i < argc; i++){
        settings->exec_argv[i] = replace_str(settings->exec_argv[i], "<path>", path);
        // if(strcmp(tok, "<path>") == 0)
        // { 
        //     args[i] = (char*)malloc(sizeof(char) * (strlen(path)+1));
        //     if(strlcpy(args[i], path, strlen(path) + 1) == NULL)
        //     {
        //         fprintf(stderr, "Failed to copy path to args.");
        //         exit(1);   
        //     } 
        // }
        // else
        // {
        args[i] = (char*)malloc(strlen(settings->exec_argv[i])+1);
        strlcpy(args[i], settings->exec_argv[i], strlen(settings->exec_argv[i])+1);
        // }

    }
   
    args[argc] = NULL;
    signal(SIGCHLD, sig_handler);

    pipe(p);
    from_child = p[0];
    to_parent = p[1];
    pid = fork();
    if(pid == 0)
    {
        close(from_child);
        close(STDIN_FILENO);
        dup2(STDOUT_FILENO, to_parent);
        dup2(STDERR_FILENO, to_parent);
        //this is child process
        if(execvp(args[0], args) == -1)
        {
            int error = errno;
            perror("Execvp function failed");
            fprintf(stderr, "Failed to execute the given program %s", settings->exec_argv[0]);
            
            exit(1);
        }
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

        
        
    }

    
    return 0;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
