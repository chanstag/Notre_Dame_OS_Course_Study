/* utilities.c */

#include "search.h"

#include <errno.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

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


/**
 * @brief print the contents of the given path   
 * 
 * @param path path to directory to print it's contents
 */
void print_directory_contents(const char *path){
    struct dirent* direct_entry;
    DIR* directoryfd = opendir(path);
    int error = errno;
    //check if opendir failed
    if(directoryfd == NULL && error != 0){
        fprintf(stderr, "could not open %s", path);
        exit(EXIT_FAILURE);
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
    struct stat fd; 
    stat(path, &fd);//we need to test this

    return fd.st_mtimespec.tv_sec;
}


/**
 * @brief 
 * 
 * @param settings      pointer to settings instance to initialize
 *
 */
void    initSettings(Settings *settings){
    settings->access = 0;
    settings->type = 0;
    settings->empty = false;
    settings->name = "";
    settings->path = "";
    settings->perm = 0;
    settings->newer = 0;
    settings->uid = -1;
    settings->gid = -1;
    settings->print = true;
    settings->exec_argc = 0;
    settings->exec_argv = NULL;
    return;
}

void get_directory_contents(const char* path){
    
    return;
}

char** alloc_stack(size_t size){
    char** stack;
    stack = (char**)malloc(sizeof(char*) * size);
    for(int i = 0; i < size; i++){
        stack[i] = (char*)malloc(sizeof(char) * 20);
    }
    if(stack != NULL){
        return stack;
    }
    else{
        exit(EXIT_FAILURE);
    }
}

int expand_stack(char** stack, size_t* size){
    char** new_stack;
    size_t new_size = *size*2;
    new_stack = (char**)malloc(sizeof(char*) * (*size*2));
    for(int i = 0; i < *size; i++){
        new_stack[i] = (char*)malloc(sizeof(char) * (strlen(stack[i]) + 1));
        strcpy(new_stack[i],  stack[i]);
        free(stack[i]);
    }
    free(stack);
    stack = new_stack;
    *size = new_size;
    return 0;
}
// 1 << 8 = 1 * 2^8  = 
enum class {USER, GROUP, OTHER};
enum permission {READ, WRITE, EXEC};
mode_t perm(enum class c, enum permission p) { return 1 << ((2-p) + (2-c)*3);} //reference: https://jameshfisher.com/2017/02/24/what-is-mode_t/

/**
 * @brief convert integer to mode_t
 * 
 * @param perm 
 * @return mode_t 
 */
mode_t int_to_mode(int perm) {
    mode_t mode = 0;
    while(perm > 10 ){
        mode = mode | (111 & perm%10);
        mode = mode << 3; 
        perm /= 10;
    }
    mode = mode | (111 & perm%10);
    
    return mode;
}
/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
