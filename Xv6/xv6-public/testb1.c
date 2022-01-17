#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc , char * argv[]){
    printf(1, "B1 Online Output\n");
    char *c = argv[1];
    int num1 = atoi(argv[2]);
    int num2 = atoi(argv[3]);

    if(strcmp(c, "add")==0){
        printf(1, "Addition of %d and %d = %d \n", num1, num2, b1addition(num1, num2));
    }
    exit();
}


//testb1 add 1 2