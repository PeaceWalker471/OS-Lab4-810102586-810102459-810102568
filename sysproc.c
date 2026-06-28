#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "rwlock.h"
#include "ticketlock.h"

extern struct ticketLock globalTicketLock;

extern struct rwlock global_rwlock;

struct spinlock print_lock;


#define NCPU 8
#define NSYSCALL 32

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern uint syscall_count[NCPU][NSYSCALL];

int
sys_getcount(void)
{
    int num;
    int total = 0;
    int i;

    if(argint(0, &num) < 0)
        return -1;

    for(i = 0; i < NCPU; i++)
        total += syscall_count[i][num];


cprintf("kernel total = %d\n", total);

    return total;
}

int
sys_getcountcpu(void)
{
    int num;
    int cpu;

    if(argint(0, &num) < 0)
        return -1;

    if(argint(1, &cpu) < 0)
        return -1;

    if(cpu < 0 || cpu >= NCPU)
        return -1;

    return syscall_count[cpu][num];
}

#define BUFFER_SIZE 16

extern int buffer[BUFFER_SIZE];
extern int head;
extern int tail;
extern int count;
extern struct spinlock buffer_lock;

char full_channel;
char empty_channel;

int
sys_produce(void)
{
    int value;

    if(argint(0, &value) < 0)
        return -1;

    acquire(&buffer_lock);

    while(count == BUFFER_SIZE){
    sleep(&full_channel, &buffer_lock);
    }

    buffer[tail] = value;

    tail = (tail + 1) % BUFFER_SIZE;

    count++;

    wakeup(&empty_channel);

    release(&buffer_lock);

    return 0;
}

int
sys_consume(void)
{
    int value;

    acquire(&buffer_lock);

    while(count == 0){
    sleep(&empty_channel, &buffer_lock);
    }

    value = buffer[head];

    head = (head + 1) % BUFFER_SIZE;

    count--;

    wakeup(&full_channel);

    release(&buffer_lock);

    return value;
}

int
sys_printsync(void)
{
    int type;
    int id;
    int value;

    if(argint(0, &type) < 0)
        return -1;

    if(argint(1, &id) < 0)
        return -1;

    if(argint(2, &value) < 0)
        return -1;

    acquire(&print_lock);

    switch(type){

    case 0:
        cprintf("[Producer %d] Produced %d\n",
                id,
                value);
        break;

    case 1:
        cprintf("[Consumer %d] Consumed %d\n",
                id,
                value);
        break;

    case 2:
        cprintf("Producer %d finished.\n",
                id);
        break;

    case 3:
        cprintf("Consumer %d finished.\n",
                id);
        break;

    case 4:
        cprintf("===== Blocking Producer-Consumer Test =====\n");
        break;

    case 5:
        cprintf("\n===== Test Finished Successfully =====\n");
        break;

    /* ---------- Reader/Writer ---------- */

    case 6:
        cprintf("===== Reader-Writer Test =====\n");
        break;

    case 7:
        cprintf("[Reader %d] Entered\n",
                id);
        break;

    case 8:
        cprintf("[Reader %d] Leaving\n",
                id);
        break;

    case 9:
        cprintf("[Writer %d] Waiting\n",
                id);
        break;

    case 10:
        cprintf("[Writer %d] Entered\n",
                id);
        break;

    case 11:
        cprintf("[Writer %d] Leaving\n",
                id);
        break;

    case 12:
        cprintf("===== Reader-Writer Test Finished =====\n");
        break;

    default:
        cprintf("Unknown printsync type: %d\n", type);
        break;
    }

    release(&print_lock);

    return 0;
}

int
sys_rwreadlock(void)
{
    rwlock_acquire_read(&global_rwlock);
    return 0;
}

int
sys_rwreadunlock(void)
{
    rwlock_release_read(&global_rwlock);
    return 0;
}

int
sys_rwwritelock(void)
{
    rwlock_acquire_write(&global_rwlock);
    return 0;
}

int
sys_rwwriteunlock(void)
{
    rwlock_release_write(&global_rwlock);
    return 0;
}

int
sys_ticketlock_acquire(void)
{
    ticketLock_acquire(&globalTicketLock);
    return 0;
}

int
sys_ticketlock_release(void)
{
    ticketLock_release(&globalTicketLock);
    return 0;
}
