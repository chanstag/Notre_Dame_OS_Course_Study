/* search.c */

#include "search.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <dirent.h>

/**
 * Recursively search through the provided root directory
 * @param   root        Path to directory
 * @param   settings    Settings structure
 * @return  Whether or not the search was successful.
 */
int search(const char *root, const Settings *settings)
{
    // DEBUG_PRINT(("%s\n", root));
    DIR *directory;
    int error;
    char *current_path;
    char *full_path;
    struct stat fd;
    bool success = false;

    current_path = (char *)malloc(sizeof(char) * strlen(root) + 1);
    full_path = (char *)malloc(sizeof(char) * 2 * strlen(root) + 1);
    strcpy(current_path, root);

    directory = opendir(current_path);
    errno = 0;
    struct dirent *entry = readdir(directory);
    if(entry  == NULL)
    {
        error = errno;
        if(error != -1){
            fprintf(stderr, "Error reading directory entry.");
        }

    }

    while (entry != NULL)
    {
        snprintf(full_path, strlen(current_path) + strlen(entry->d_name) + 2, "%s/%s", current_path, entry->d_name);
        if (lstat(full_path, &fd) < 0)
        {
            error = errno;
            fprintf(stderr, "Error occured in stating: %s.", full_path);
            if (error == EACCES)
            {
                fprintf(stderr, "Search permission is denied for a component of the path prefix");
            }
            else if (error == EIO)
            {
                fprintf(stderr, "An error occurred while reading from the file system.");
            }
            else if (error == ELOOP)
            {
                fprintf(stderr, "Too many symbolic links were encountered in resolving %s.", full_path);
            }
            else if (error == ENAMETOOLONG)
            {
                // fprintf(stderr, "The length of the path argument exceeds {%d} or a pathname component is longer than {%d}.", _PC_PATH_MAX, _PC_NAME_MAX);
            }
            else if (error == ENOENT)
            {
                fprintf(stderr, "A component of path does not name an existing file or path is an empty string.");
            }
            else if (error == ENOTDIR)
            {
                fprintf(stderr, "A component of the path prefix is not a directory.");
            }
            else if (error == EOVERFLOW)
            {
                fprintf(stderr, "The file size in bytes or the number of blocks allocated to the file or the file serial number cannot be represented correctly in the structure pointed to by buf.");
            }
            exit(EXIT_FAILURE);
        }
        else
        {
            if (S_ISDIR(fd.st_mode))
            {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                // if(fnmatch(dirname(copy_path), settings->path, FNM_PATHNAME) == 0)
                {

                }
                else
                {
                    if(search(full_path, settings) == true)
                    {
                        success = true;
                    }
                }
            }
            // call filter function on this file to see if it should be included in output
            if(!filter(full_path, settings))
            {
                // execute(full_path, settings);
                printf("%s\n", full_path);
                success = true;
            }
            
        }
        strcpy(full_path, current_path);
        entry = readdir(directory);
        if(entry == NULL)
        {
            error = errno;
            if(error == EIO){
                fprintf(stderr, "Error reading directory entry.");
            }
            else if (error == EFAULT)
            {
                fprintf(stderr, "Error reading directory entry.");
            }
            else if (error == EBADF)
            {
                fprintf(stderr, "Error reading directory entry.");
            }
        }

        
        

    }

    if (entry == NULL && error != 0)
    {
        fprintf(stderr, "Failed to read directory entry.");
    }
    // else if (entry == NULL && error == 0)
    // {
        
    // }
    

    return success;
}

    /* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
