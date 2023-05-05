/* driver.c */

#include "search.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>

#include <assert.h>

/* Functions */

void	    usage(const char *program_name, int status) {
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
Settings* parseArgs(int argc,char *argv[]){
    enum options opts;
    Settings* settings = (Settings*)malloc(sizeof(Settings));
    if(settings == NULL){
        fprintf(stderr, "Failed to allocated memory. On line: %d", __LINE__);
        return NULL;
    }

    initSettings(settings);
    int i = 2;
    int len_options = sizeof(options_vec)/sizeof(char*);
    bool valid_option = false;
    while(i < argc){
        valid_option = false;
        for(int j = 0; j < len_options; j++){
            if(strcmp(argv[i], options_vec[j]) == 0){
                valid_option = true;
                opts = j;
                 switch(opts){
                    case executable:
                        settings->access = X_OK;
                        settings->type = 1;
                        i++;
                        break;
                    case readable:
                        settings->access = R_OK;
                        settings->type = 1;
                        i++;
                        break;
                    case writeable:
                        settings->access = W_OK;
                        settings->type = 1;
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
                        settings->name = "*.txt"; //will need to fix this
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
bool compareSettings(Settings* s1, Settings* s2){
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

void test(){
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
    if(compareSettings(&correct_settings, parsed_settings) != true)
    {
        fprintf(stderr, "Test 3 failed as expected.\n");
        test_passed = true;
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

int	main(int argc, char *argv[]) {

    if(argc < 1)
    {
        usage(argv[0], 1);
    }
    if(argc==2 && strcmp(argv[1], "-test") == 0)
    {
        test();
    }
    else if (argc==1)
    {
        print_directory_contents(argv[1]);
    }
    else{
        parseArgs(argc, argv);
    }
    return EXIT_SUCCESS;
}

/* vim: set sts=4 sw=4 ts=8 expandtab ft=c: */
