#include "types.h"
#include "stat.h"
#include "user.h"

#define NCHILD 8
#define NITER 500000

int
main(void)
{
    int i, pid;
    int expected;

    expected = NCHILD * NITER;

    printf(1, "Starting test...\n");

    for(i = 0; i < NCHILD; i++){
        pid = fork();

        if(pid < 0){
            printf(1, "fork failed\n");
            exit();
        }

        if(pid == 0){
            int j;

            for(j = 0; j < NITER; j++){
                getpid();
            }

            exit();
        }
    }

    while(wait() > 0)
        ;

    printf(1, "Expected getpid count = %d\n", expected);

    // 11 = SYS_getpid
    printf(1, "Actual getpid count   = %d\n",
           getcount(11));

    exit();
}