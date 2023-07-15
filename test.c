#include <sys/stat.h>

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
    return 0;
}