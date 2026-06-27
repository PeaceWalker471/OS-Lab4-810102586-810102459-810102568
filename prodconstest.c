#include "types.h"
#include "stat.h"
#include "user.h"
#define NPRODUCER 2
#define NCONSUMER 2
#define NITEMS    20
int
main(void)
{
    int i, j;
    int pid;
    printf(1, "===== Producer-Consumer Test =====\n");
    for(i = 0; i < NPRODUCER; i++){
        pid = fork();

        if(pid == 0){
            int produced = 0;
            int failed = 0;

            for(j = 0; j < NITEMS; j++){

                while(produce(i * 100 + j) < 0)
                    sleep(1);

                produced++;
            }

            printsync(0, i, produced);

            exit();
        }
    }

    for(i = 0; i < NCONSUMER; i++){
        pid = fork();

        if(pid == 0){

            int consumed = 0;
            int value;

            for(j = 0; j < NITEMS; j++){

                while((value = consume()) < 0)
                    sleep(1);

                consumed++;
            }

            printsync(1, i, consumed);

            exit();
        }
    }

    while(wait() > 0)
        ;

    printf(1, "\n===== Test Finished =====\n");
    printf(1, "Expected Produced : %d\n", NPRODUCER * NITEMS);
    printf(1, "Expected Consumed : %d\n", NCONSUMER * NITEMS);

    exit();
}