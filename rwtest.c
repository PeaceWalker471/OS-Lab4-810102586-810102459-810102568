#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
    int pid;

    printsync(6, 0, 0);

    /* ---------------- Reader 0 ---------------- */

    pid = fork();

    if(pid == 0){

        rwreadlock();

        printsync(7, 0, 0);

        sleep(100);

        printsync(8, 0, 0);

        rwreadunlock();

        exit();
    }

    /* ---------------- Reader 1 ---------------- */

    pid = fork();

    if(pid == 0){

        rwreadlock();

        printsync(7, 1, 0);

        sleep(100);

        printsync(8, 1, 0);

        rwreadunlock();

        exit();
    }

    /*
     * کمی صبر می‌کنیم تا هر دو Reader
     * ابتدا وارد بخش بحرانی شوند.
     */

    sleep(20);

    /* ---------------- Writer ---------------- */

    pid = fork();

    if(pid == 0){

        printsync(9, 0, 0);

        rwwritelock();

        printsync(10, 0, 0);

        sleep(100);

        printsync(11, 0, 0);

        rwwriteunlock();

        exit();
    }

    while(wait() > 0)
        ;

    printsync(12, 0, 0);

    exit();
}