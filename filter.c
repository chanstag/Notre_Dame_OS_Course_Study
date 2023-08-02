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
#include <libgen.h>


/**
 * Test whether or not the given path should be filtered out of the output.
 * @param   path        Path to file or directory.
 * @param   settings    Settings structure.
 * @return  Whether or not the path should be excluded from the output (true to
 * exclude, false to include).
 */
bool        filter(const char *path, const Settings *settings) {
    struct stat fd;
    char* copy_path;
    copy_path = (char*) malloc(sizeof(char) * strlen(path)+1);
    strcpy(copy_path, path);

    if(lstat(copy_path, &fd) < 0)
    {
       int error = errno;
       fprintf(stderr, "Error occured in stating: %s.", copy_path);
       if(error == EACCES)
       {
            fprintf(stderr, "Search permission is denied for a component of the path prefix");
       }
       else if(error == EIO)
       {
            fprintf(stderr, "An error occurred while reading from the file system.");
       }
       else if(error == ELOOP)
       {
            fprintf(stderr, "Too many symbolic links were encountered in resolving %s.", copy_path);
       }
       else if(error == ENAMETOOLONG)
       {
            fprintf(stderr, "The length of the path argument exceeds {%d} or a pathname component is longer than {%d}.", _PC_PATH_MAX, _PC_NAME_MAX);
       }
       else if(error == ENOENT)
       {
            fprintf(stderr, "A component of path does not name an existing file or path is an empty string.");
       }
       else if(error == ENOTDIR)
       {
            fprintf(stderr, "A component of the path prefix is not a directory.");
       }
       else if(error == EOVERFLOW)
       {
            fprintf(stderr, "The file size in bytes or the number of blocks allocated to the file or the file serial number cannot be represented correctly in the structure pointed to by buf.");
       }
       exit(EXIT_FAILURE);
    }
    else
    {
          //test for access mode
           if(faccessat(AT_FDCWD, copy_path, settings->access, AT_SYMLINK_NOFOLLOW) < 0)
           {
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
           else
           {
               
           }
          //
          if(settings->type == 2 && S_ISDIR(fd.st_mode) != 0)
          {
               return false;
          }
          if(settings->empty && S_ISDIR(fd.st_mode)){
               
          }
          if(settings->empty && fd.st_size == 0)
          {
               return false;
          }
          if(settings->name)
          {
               if(strcmp(settings->name, basename(copy_path)) == 0)//
               {
                    return false;
               }

          }
          if(settings->path)
          {
               // printf("dirname: %s", dirname(copy_path));
               if(fnmatch(dirname(copy_path), settings->path, FNM_PATHNAME) == 0)//check if directory name matches pattern
               {
                    return false;
               }
          }
          if((int_to_mode(settings->perm) == fd.st_mode))//Check if permissions match settings
          {
               return false;
          }
          if(settings->newer != -1 && settings->newer < fd.st_mtimespec.tv_sec){//check if file is newer than setting time
               return false;
          }
          if(settings->uid == fd.st_uid)
          {
               return false;
          }
          if(settings->gid == fd.st_gid)
          {
               return false;
          }
          else
          {
               return true;
          }


        
    }
    return false;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
