#include "types.h"
#include "stat.h"
#include "user.h"

int main(){
    printf(1, "i am calling system call to get process size. size = %d \n", getsize());
    exit();
}