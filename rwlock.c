#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "rwlock.h"

void
rwlock_init(struct rwlock *rw)
{
    initlock(&rw->lock, "rwlock");

    rw->readers = 0;
    rw->active_writer = 0;
    rw->waiting_writers = 0;
}

void
rwlock_acquire_read(struct rwlock *rw)
{
    acquire(&rw->lock);

    while(rw->active_writer || rw->waiting_writers)
        sleep(rw, &rw->lock);

    rw->readers++;

    release(&rw->lock);
}

void
rwlock_release_read(struct rwlock *rw)
{
    acquire(&rw->lock);

    rw->readers--;

    if(rw->readers == 0)
        wakeup(rw);

    release(&rw->lock);
}

void
rwlock_acquire_write(struct rwlock *rw)
{
    acquire(&rw->lock);

    rw->waiting_writers++;

    while(rw->active_writer || rw->readers > 0)
        sleep(rw, &rw->lock);

    rw->waiting_writers--;

    rw->active_writer = 1;

    release(&rw->lock);
}

void
rwlock_release_write(struct rwlock *rw)
{
    acquire(&rw->lock);

    rw->active_writer = 0;

    wakeup(rw);

    release(&rw->lock);
}