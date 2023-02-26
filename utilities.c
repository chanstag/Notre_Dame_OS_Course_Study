/* utilities.c */

#include "search.h"

#include <errno.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Test whether or not a directory is empty.
 * @param   path        Path to directory.
 * @return  Whether or not a directory is empty.
 */
bool        is_directory_empty(const char *path) {
    struct dirent* directory_entry;
    DIR* directory;
    directory = opendir(path);

    if(directory == NULL){
        fprintf(stderr, "The directory could not be opened. Please provide a valid directory path.");
        return true;
    }
    else{
        directory_entry = readdir(directory);
        int error = errno;
        if(directory_entry != NULL){
            return false;
        }
        if(directory_entry == NULL){
                if(error == EACCES){
                    fprintf(stderr, "Do not have permission to open this directory");
                }
                else if(error == ENAMETOOLONG){
                    fprintf(stderr, "Filename is too long.");
                    
                }
                return true;
        }
        
        
    }
    return false;
}

void printDirectoryContents(const char *path){
    struct dirent* direct_entry;
    DIR* directoryfd = opendir(path);
    //check if opendir failed
    if(directoryfd == NULL){
        fprintf(stderr, "could not open %s", path);
    }
    direct_entry = readdir(directoryfd);
    while(direct_entry != NULL){
        printf("|");
        printf(" -> %s\n", direct_entry->d_name);
        direct_entry = readdir(directoryfd);
    }
}


/**
 * Retrieve the modification time of the given file.
 * @param   path        Path to file of directory.
 * @return  The modification time of the given file.
 */
time_t      get_mtime(const char *path) {
    return 0;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
