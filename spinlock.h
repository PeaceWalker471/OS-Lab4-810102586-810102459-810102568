#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

struct spinlock {
  uint locked;

  char *name;
  struct cpu *cpu;
  uint pcs[10];
};

#endif


