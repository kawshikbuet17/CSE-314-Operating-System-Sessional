#include "types.h"
#include "stat.h"
#include "user.h"

int main(){
    printf(1, "B2 Online Output\n");

    int studentId = kawshik();
    int randomNum = paul();

    int n = studentId;
    int sum = 0;
    while(n>0){
        sum += n%10;
        n = n/10;
    }

    printf(1, "Student ID: %d\nSum: %d Random Number: %d, Remainder: %d\n", studentId, sum, randomNum, sum%randomNum);
    
    exit();
}
