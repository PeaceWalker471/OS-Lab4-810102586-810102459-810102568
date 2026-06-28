#include "types.h"
#include "stat.h"
#include "user.h"

#define NPROC 5

int
main(void)
{
    int i;
    int pid;
    volatile int j;

    printf(1,
           "\n===== Ticket Lock Test =====\n\n");

    for(i = 0; i < NPROC; i++){

        pid = fork();

        if(pid == 0){

            ticketlock_acquire();

            for(j = 0; j < 2000; j++)
                ;

            ticketlock_release();

            exit();
        }

        sleep(1);
    }

    while(wait() > 0)
        ;

    printf(1,
           "\n===== Test Finished =====\n");

    exit();
}