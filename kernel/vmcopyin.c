#include "param.h"
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"

//
// This file contains copyin_new() and copyinstr_new(), the
// replacements for copyin and coyinstr in vm.c.
//

static struct stats {
  int ncopyin;
  int ncopyinstr;
} stats;

int
statscopyin(char *buf, int sz) {
  int n;
  n = snprintf(buf, sz, "copyin: %d\n", stats.ncopyin);
  n += snprintf(buf+n, sz, "copyinstr: %d\n", stats.ncopyinstr);
  return n;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
int
copyin_new(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;
  struct proc *p = myproc();

  // If we have a current proc, do a quick bounds check using p->sz.
  // But don't assume myproc() is always non-NULL — fall back to walk checks.
  if (p) {
    if (srcva >= p->sz || srcva + len > p->sz || srcva + len < srcva)
      return -1;
  }

  while (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    if (pa0 == 0)
      return -1;
    n = PGSIZE - (srcva - va0);
    if (n > len)
      n = len;
    memmove(dst, (void *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }

  stats.ncopyin++;   // XXX: no lock around stats update (跟原答案一致)
  return 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
int
copyinstr_new(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
  uint64 va0, pa0;
  struct proc *p = myproc();

  // If we have a current proc, limit to p->sz. If not, we'll check via walkaddr below.
  if (p) {
    if (srcva >= p->sz)
      return -1;
  }

  while (max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    if (pa0 == 0)
      return -1;
    char *ptr = (char *)(pa0 + (srcva - va0));
    uint64 n = PGSIZE - (srcva - va0);
    if (n > max)
      n = max;

    for (uint64 i = 0; i < n; i++) {
      char c = ptr[i];
      dst[i] = c;
      if (c == '\0') {
        stats.ncopyinstr++;   // XXX: no lock
        return 0;
      }
    }

    dst += n;
    max -= n;
    srcva = va0 + PGSIZE;
    // loop continues
  }

  return -1;
}
