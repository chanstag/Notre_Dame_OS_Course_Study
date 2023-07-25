#include <sys/stat.h>
#include <fnmatch.h>
#include <stdio.h>
#include <libgen.h>

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

int main(){
    mode_t mode = int_to_mode(777);
    if(fnmatch("/Users/chanstag/Systems_Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder", dirname("/Users/chanstag/Systems_Programming/Notre_Dame_Courses/cse-20289-sp17-project01/Test_Folder/."), FNM_PATHNAME) == 0)//check if directory name matches pattern
    {
        // return false;
        printf("Matches");
    }
    return 0;
}