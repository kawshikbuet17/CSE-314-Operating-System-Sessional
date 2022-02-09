#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "pstat.h"

int main(int argc, char * argv[])
{
    int pid;
    int n = atoi(argv[1]);
    int pidArr[n];
    settickets(50);

    for(int i = 0; i < n; i++) {
        pid = fork();
        if(pid < 0) {
            // this is the error branch
            printf(1, "process : %d ; failed to reproduce\n", getpid());
        }
        else if(pid > 0) {
            pidArr[i] = pid;
            // this is the parent branch
            printf(1, "process : %d ; created child : %d\n", getpid(), pid);
            // wait(); // wait for the children to return
        }
        else {
            // this is the children branch
            printf(1, "process : %d ; is a child\n", getpid());
            int t = 1;
            if(argc >= 2) {
                t = atoi(argv[i+2]);
                
            }
            if(i%2!=0){
                settickets(t);
                printf(1, "%d tickets set to child : %d", t, getpid());
            }
            else{
                printf(1, "parent's tickets set to child : %d", getpid());
            }

            for(;;) {
                ;
                // do nothing
            }
            
        }
        
    }
    int x = 0;
    for(int k = 0; k<=100000000; k++) {
        x = x-k + 3.14*89.64;
        // do nothing
    }
    printf(1, "\nx=%d\n", x);

    struct pstat *p = malloc (sizeof(struct pstat));

    printf(1, "\nBEFORE\n");
    int check;
    check = getpinfo(p);
    // printf(1, "check %d \n", check);
    printf(1, "Id\t\tTickets\t\t Ticks\n");
    if(check == 0){
        for(int i = 0; i < NPROC; i++){
            if(p->inuse[i])
                printf(1, "%d \t\t %d \t\t %d\n", p->pid[i], p->tickets[i], p->ticks[i]);
            
        }
    }

    for(int k = 0; k<=100000000; k++) {
        x = x+k - 3.14*89.64;
        // do nothing
    }
    printf(1, "\nx=%d\n", x);

    struct pstat *p1 = malloc (sizeof(struct pstat));

    printf(1, "\nAFTER\n");
    check = getpinfo(p1);
    // printf(1, "check %d \n", check);
    printf(1, "Id\t\tTickets\t\t Ticks\n");
    if(check == 0){
        for(int i = 0; i < NPROC; i++){
            if(p1->inuse[i])
                printf(1, "%d \t\t %d \t\t %d\n", p1->pid[i], p1->tickets[i], p1->ticks[i]-p->ticks[i]);
            
        }
    }


    for (int i = 0; i < n; ++i) {
        kill(pidArr[i]);
    }
    for (int i = 0; i < n; ++i) {
        wait();
    }
    exit();
}