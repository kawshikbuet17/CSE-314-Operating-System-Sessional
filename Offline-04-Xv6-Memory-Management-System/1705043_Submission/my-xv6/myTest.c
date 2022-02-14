#include "types.h"
#include "stat.h"
#include "user.h"


void dummy(){


    int sz = 4096*17;
    int pid = fork();
    int k;
    if(pid == 0){
        k = 17;
    }
    else{
        k = 13;
    }

    int *mem = (int*) malloc(sz);
    for(int i = 0; i < sz/4; i++){
        mem[i] = k * i;
    }
    sleep(250);

    int ok = 1;
    for(int i = 0; i < sz/4; i++){
        if(mem[i] != k*i){
            ok = 0;
        }
    }
    sleep(250);

    if(!ok){
        printf(2, "failed\n");
    }
    else
        printf(2, "success\n");

    free((void*) mem);

    if(pid != 0)
        wait();


}

int main(int argc, char * argv[]){

    dummy();
    exit();
}