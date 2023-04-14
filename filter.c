/* filter.c */

#include "search.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <dirent.h>
#include <fnmatch.h>
#include <sys/stat.h>
#include <unistd.h>
#include <_types.h>
#include <stdlib.h>

/**
 * Test whether or not the given path should be filtered out of the output.
 * @param   path        Path to file or directory.
 * @param   settings    Settings structure.
 * @return  Whether or not the path should be excluded from the output (true to
 * exclude, false to include).
 */
bool        filter(const char *path, const Settings *settings) {
    struct stat fd;
    if(lstat(path, &fd) < 0){
       int error = errno;
       fprintf(stderr, "Error occured in stating: %s.", path);
       if(error == EACCES){
            fprintf(stderr, "Search permission is denied for a component of the path prefix");
       }
       else if(error == EIO){
            fprintf(stderr, "An error occurred while reading from the file system.");
       }
       else if(error == ELOOP){
            fprintf(stderr, "Too many symbolic links were encountered in resolving %s.", path);
       }
       else if(error == ENAMETOOLONG){
            fprintf(stderr, "The length of the path argument exceeds {%d} or a pathname component is longer than {%d}.", _PC_PATH_MAX, _PC_NAME_MAX);
       }
       else if(error == ENOENT){
            fprintf(stderr, "A component of path does not name an existing file or path is an empty string.");
       }
       else if(error == ENOTDIR){
            fprintf(stderr, "A component of the path prefix is not a directory.");
       }
       else if(error == EOVERFLOW){
            fprintf(stderr, "The file size in bytes or the number of blocks allocated to the file or the file serial number cannot be represented correctly in the structure pointed to by buf.");
       }
       exit(EXIT_FAILURE);
    }
    else{
          //test for access mode
           if(faccessat(AT_FDCWD, path, settings->access, AT_SYMLINK_NOFOLLOW) < 0){
               int error = errno;
               if(error == EACCES){
                    fprintf(stderr, "Access denied for faccessat.");
               }
               else if(error == EFAULT){

               }
               else if(error == EINVAL){
                    fprintf(stderr, "Mode was incorrectly set for faccessat.");
               }
               else if(error == EIO){
                    fprintf(stderr, "An I/O error occurred.");
               }
               return true;
           }
           else if(){

           }
        
    }
    return false;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
