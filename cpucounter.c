#include "types.h"
#include "stat.h"
#include "user.h"
#define NCHILD 8
#define NITER 100000
int
main(void)
{
    int i,j;

    for(i=0;i<NCHILD;i++){
        if(fork()==0){

            for(j=0;j<NITER;j++)
                getpid();

            exit();
        }
    }
    while(wait() > 0)
        ;

    for(i=0;i<7;i++){
        printf(1,
               "CPU %d getpid count: %d\n",
               i,
               getcountcpu(11, i));
    }

    printf(1,
           "Total getpid count: %d\n",
           getcount(11));

    printf(1,
           "Expected count: %d\n",
           NCHILD * NITER);

    exit();
}