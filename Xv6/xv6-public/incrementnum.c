#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc , char * argv[]){
    printf(1, "Incrementing %s\n", argv[1]);
    printf(1 , "Answer = %d\n" , incrementnum(atoi(argv[1])));
    exit();
}