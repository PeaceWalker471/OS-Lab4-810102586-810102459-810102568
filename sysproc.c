#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

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

int
sys_produce(void)
{
    int value;

    if(argint(0, &value) < 0)
        return -1;

    acquire(&buffer_lock);

    if(count == BUFFER_SIZE){
        release(&buffer_lock);
        return -1;
    }

    buffer[tail] = value;

    tail = (tail + 1) % BUFFER_SIZE;

    count++;

    release(&buffer_lock);

    return 0;
}

int
sys_consume(void)
{
    int value;

    acquire(&buffer_lock);

    if(count == 0){
        release(&buffer_lock);
        return -1;
    }

    value = buffer[head];

    head = (head + 1) % BUFFER_SIZE;

    count--;

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

    if(type == 0)
        cprintf("Producer %d finished (Produced = %d)\n",
                id,
                value);
    else
        cprintf("Consumer %d finished (Consumed = %d)\n",
                id,
                value);

    release(&print_lock);

    return 0;
}
