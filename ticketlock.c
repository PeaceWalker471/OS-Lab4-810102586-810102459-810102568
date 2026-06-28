#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "ticketlock.h"

void
ticketLock_init(struct ticketLock *tl)
{
    tl->ticket = 0;

    tl->turn = 0;

    initlock(&tl->lock, "ticketlock");
}

void
ticketLock_acquire(struct ticketLock *tl)
{
    uint myticket;

    acquire(&tl->lock);

    myticket = tl->ticket;
    tl->ticket++;

    cprintf("PID %d requested Ticket %d\n",
            myproc()->pid,
            myticket);

    release(&tl->lock);

    while(myticket != tl->turn)
        ;

    cprintf("PID %d ENTERED  Ticket=%d  Turn=%d\n",
            myproc()->pid,
            myticket,
            tl->turn);
}

void
ticketLock_release(struct ticketLock *tl)
{
    acquire(&tl->lock);

    tl->turn++;

    release(&tl->lock);
}