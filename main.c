/* driver.c */

#include "search.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

/* Functions */

void        usage(const char *program_name, int status) {
    printf("Enter usage\n");
    fprintf(stderr, "Usage: %s PATH [OPTIONS] [EXPRESSION]\n", program_name);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "    -executable     File is executable or directory is searchable to user\n");
    fprintf(stderr, "    -readable       File readable to user\n");
    fprintf(stderr, "    -writable       File is writable to user\n\n");
    fprintf(stderr, "    -type [f|d]     File is of type f for regular file or d for directory\n\n");
    fprintf(stderr, "    -empty          File or directory is empty\n\n");
    fprintf(stderr, "    -empty          File or directory is empty\n\n");
    fprintf(stderr, "    -name  pattern  Base of file name matches shell pattern\n");
    fprintf(stderr, "    -path  pattern  Path of file matches shell pattern\n\n");
    fprintf(stderr, "    -perm  mode     File's permission bits are exactly mode (octal)\n");
    fprintf(stderr, "    -newer file     File was modified more recently than file\n\n");
    fprintf(stderr, "    -uid   n        File's numeric user ID is n\n");
    fprintf(stderr, "    -gid   n        File's numeric group ID is n\n");
    fprintf(stderr, "\nExpressions:\n\n");
    fprintf(stderr, "    -print          Display file path (default)\n");
    fprintf(stderr, "    -exec cmd {} ;  Execute command on path\n");
    exit(status);
}

char const *options_vec[] = {"-executable", "-readable", "-writeable", "-type", "-empty", "-name", "-path", "-perm", "-newer", "-uid", "-gid"};

enum options{
    executable,
    readable,
    writeable,
    type,
    empty,
    name,
    path,
    perm,
    newer,
    uid,
    gid
};
/**
 * @brief Function to parse the input parameters and set the appropriate settings in settings struct
 * 
 * @param argc 
 * @param argv 
 * @return Settings* 
 */
Settings*       parseArgs(int argc,char *argv[]){
    enum options opts;
    bool valid_option = false;

    Settings* settings = (Settings*)malloc(sizeof(Settings));
    if(settings == NULL){
        fprintf(stderr, "Failed to allocated memory. On line: %d", __LINE__);
        return NULL;
    }

    initSettings(settings);
    int i = 2;
    int len_options = sizeof(options_vec)/sizeof(char*);
    
    while(i < argc){
        valid_option = false;
        for(int j = 0; j < len_options; j++){
            if(strcmp(argv[i], options_vec[j]) == 0){
                valid_option = true;
                opts = j;
                 switch(opts){
                    case executable:
                        settings->access = X_OK | settings->access;
                        // settings->type = 1;
                        i++;
                        break;
                    case readable:
                        settings->access = R_OK | settings->access;
                        // settings->type = 1;
                        i++;
                        break;
                    case writeable:
                        settings->access = W_OK | settings->access;
                        // settings->type = 1;
                        i++;
                        break;
                    case type:
                        if(strcmp(argv[i+1], "f") == 0){
                            settings->type = 1;
                        }
                        else if(strcmp(argv[i+1], "d") == 0){
                            settings->type = 2;
                        }
                        else{
                            fprintf(stderr, "Please provide a valid parameter of either 'f' or 'd'\n");
                            return NULL;
                        }
                        i += 2;
                        break;
                    case empty:
                        settings->empty = true;
                        i++;
                        break;
                    case name:
                        //idk how to handle shell patterns yet
                        settings->name = argv[i+1]; //will need to fix this
                        i += 2;
                        break;
                    case path:
                        settings->path = argv[i+1];
                        i += 2;
                        break;
                    case perm:
                        sscanf(argv[i+1], "%d", &(settings->perm));
                        i += 2;
                        break;
                    case newer:
                        settings->newer = get_mtime(argv[i+1]);
                        i += 2;
                        break;
                    case uid:
                        settings->uid = atoi(argv[i+1]);
                        i += 2;
                        break;
                    case gid:
                        settings->gid = atoi(argv[i+1]);
                        i += 2;
                        break;
                    default:
                        settings->empty = false;
                        i++;
                        break;
                 }
                break;
            }
        }
        if(!valid_option){
            fprintf(stderr, "Please provide a valid option.\n");
            return NULL;
        }
       
    }

    return settings;
}
/**
 * @brief Compare 2 settings structs to see if they are equal. returns true if they are equal; otherwise, returns false
 * 
 * @param s1 
 * @param s2 
 * @return true 
 * @return false 
 */
bool        compareSettings(Settings* s1, Settings* s2){
    if(s1 == NULL || s2 == NULL){
        return false;
    }
    if(s1->access != s2->access){
        return false;
    }
    else if(s1->empty != s2->empty){
        return false;
    }
    else if(s1->path != s2->path){
        return false;
    }
    else if(s1->type != s2->type){
        return false;
    }
    else if((s1->name == NULL && s2->name != NULL) || (s1->name != NULL && s2->name == NULL)){
        return false;
            
    }
    else if(s1->name != NULL && s2->name != NULL){
        if(strcmp(s1->name, s2->name) != 0){
            return false;
        }
    }
    else if(s1->newer != s2->newer){
        return false;
    }
    else if(s1->uid != s2->uid){
        return false;
    }
    else if(s1->gid != s2->gid){
        return false;
    }
    return true;
}

void        test(){
    bool test_passed = true;
    //test 0
    Settings correct_settings;
    Settings* parsed_settings;
    initSettings(&correct_settings);
    correct_settings.type = 1;
    // correct_settings.name = "*.txt";
    correct_settings.access = X_OK;
    char *args0[] = {"search", "./", "-executable"};
    parsed_settings = parseArgs(3 , args0);
    if(compareSettings(&correct_settings, parsed_settings) != true)
    {
        fprintf(stderr, "Test 0 failed\n");
        test_passed = false;
    }

    //test 1
    correct_settings.access = R_OK;
    char *args1[] = {"search", "./", "-readable"};
    parsed_settings = parseArgs(3, args1);
    if(compareSettings(&correct_settings, parsed_settings) != true)
    {
        fprintf(stderr, "Test 1 failed.\n");
        test_passed = false;
    }

    //test 2
    correct_settings.access = W_OK;
    char *args2[] = {"search", "./", "-writeable"};
    parsed_settings = parseArgs(3, args2);
    if(compareSettings(&correct_settings, parsed_settings) != true)
    {
        fprintf(stderr, "Test 2 failed.\n");
        test_passed = false;
    }

    //test 3 (intentional fail)
    correct_settings.access = W_OK;
    char *args3[] = {"search", "./", "-w"};
    parsed_settings = parseArgs(3, args3);
    if(compareSettings(&correct_settings, parsed_settings) == true) // this test should fail
    {
        fprintf(stderr, "Test 3 passed, but should have failed.\n");
        test_passed = false;
    }

    //Test 4 filter.c settings->newer return false
    Settings test_filter_settings;
    initSettings(&test_filter_settings);
    test_filter_settings.newer = 1680895800;
    test_filter_settings.type = 1;
    if(filter("./Test_Folder/file_two", &test_filter_settings) && filter("./Test_Folder/file_three", &test_filter_settings))
    {
        fprintf(stderr, "Test 4 failed.\n");
        test_passed = false;
    }

    //test 5 filter.c settings->newer return true
    if(!filter("./Test_Folder/file_one", &test_filter_settings))
    {
        fprintf(stderr, "Test 5 failed.\n");
        test_passed = false;
    }

    //test 6 filter.c settings->empty return true
    test_filter_settings.newer = -1;
    test_filter_settings.type = 1;
    test_filter_settings.empty = true;
    if(filter("./Test_Folder/file_one", &test_filter_settings))
    {
        fprintf(stderr, "Test 6 failed.\n");
        test_passed = false;
    }

    //test 7 search.c
    Settings test_search_settings;
    test_search_settings.access = R_OK;
    test_search_settings.type = 2;
    test_search_settings.name = NULL;
    test_search_settings.empty = false;
    test_search_settings.newer = -1;
    test_search_settings.path = "/Users/chanstag/Systems_Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder";
    if(!search("/Users/chanstag/Systems_Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder", &test_search_settings))
    {
        fprintf(stderr, "Test 7 failed.\n");
        test_passed = false;
    }

    //test 8 search.c failure
    test_search_settings.path = "/Users/chanstag/Systems Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder_2";
    test_search_settings.type = 0;
    test_search_settings.name = NULL;
    test_search_settings.empty = false;
    test_search_settings.newer = -1;
    if(search("/Users/chanstag/Systems_Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder", &test_search_settings))
    {
        fprintf(stderr, "Test 8 passed, but should have failed.\n");
        test_passed = false;
    }

    Settings test_exec_settings;
    const char* path = "/Users/chanstag/Systems_Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder";\
    char* args[] = {"sh", "-c", "ls <path> | wc -l"}; //must invoke this way because a pipe, | , is techinally 2 processes
    test_exec_settings.exec_argv = args;
    test_exec_settings.exec_argc = 3;
    if(execute(path, &test_exec_settings))
    {
        fprintf(stderr, "Test 9 failed.\n");
        test_passed = false;
    }

    if(!test_passed)
    {
        fprintf(stderr, "Test suite failed.\n");
        exit(1);
    }
    else
    { 
        fprintf(stdout, "All tests passed.\n");
        exit(0);
    }

}

/* Main Execution */

int	main(int argc, char **argv) {
    if(argc == 1)
    {
        usage(argv[0], 1);
    }
    if(argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-help") == 0))
    {
        usage(argv[0], 1);
    }
    if(argc==2 && strcmp(argv[1], "-test") == 0)
    {
        test();
    }
    // else if (argc==2)
    // {
    //     print_directory_contents(argv[1]);
    // }
    else{
        Settings *s = parseArgs(argc, argv);
        if(s == NULL){ // settings should not be NULL here
            return EXIT_FAILURE;
        }
        search(argv[1], s);
    }
    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
