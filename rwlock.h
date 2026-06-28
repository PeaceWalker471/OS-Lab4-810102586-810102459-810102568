#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "spinlock.h"

struct rwlock {
    struct spinlock lock;

    int readers;
    int active_writer;
    int waiting_writers;
};

void rwlock_init(struct rwlock *);
void rwlock_acquire_read(struct rwlock *);
void rwlock_release_read(struct rwlock *);
void rwlock_acquire_write(struct rwlock *);
void rwlock_release_write(struct rwlock *);

#endif