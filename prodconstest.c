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

    printsync(4, 0, 0);

    /* Producer Processes */
    for(i = 0; i < NPRODUCER; i++){
        pid = fork();

        if(pid == 0){

            for(j = 0; j < NITEMS; j++){

                produce(i * 100 + j);

                printsync(0, i, i * 100 + j);
            }

            printsync(2, i, 0);

            exit();
        }
    }

    /* Consumer Processes */
    for(i = 0; i < NCONSUMER; i++){
        pid = fork();

        if(pid == 0){

            int value;

            for(j = 0; j < NITEMS; j++){

                value = consume();

                printsync(1, i, value);
            }

            printsync(3, i, 0);

            exit();
        }
    }

    while(wait() > 0)
        ;

    printsync(5, 0, 0);

    exit();
}