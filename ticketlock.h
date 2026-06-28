#ifndef _TICKETLOCK_H_
#define _TICKETLOCK_H_

#include "types.h"
#include "spinlock.h"

struct ticketLock {

    uint ticket;

    uint turn;

    struct spinlock lock;
};

void ticketLock_init(struct ticketLock *);
void ticketLock_acquire(struct ticketLock *);
void ticketLock_release(struct ticketLock *);

#endif