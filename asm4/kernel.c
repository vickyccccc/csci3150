#include "kernel.h"

/*
  1. Check if a free process slot exists and if the there's enough free space (check allocated_pages).
  2. Alloc space for page_table (the size of it depends on how many pages you need) and update allocated_pages.
  3. The mapping to kernel-managed memory is not built up, all the PFN should be set to -1 and present byte to 0.
  4. Return a pid (the index in MMStruct array) which is >= 0 when success, -1 when failure in any above step.
*/
int proc_create_vm(struct Kernel *kernel, int size)
{
  /* Fill Your Code Below */

  // ! demo.c (line 26)
  if (size > VIRTUAL_SPACE_SIZE)
    return -1;

  // ! 1.1 Check if a free process slot exists (check the running, the slot will be the pid returned).
  int pid = -1;
  for (int i = 0; i < MAX_PROCESS_NUM; i++)
  {
    if (kernel->running[i] == 0)
    {
      pid = i;
      break;
    }
  }

  if (pid == -1)
    return -1;

  // ! 1.2 Check if there's enough free space (check allocated_pages).
  // ! e.g. if size=33 and PAGE_SIZE=32, then you need 2 pages
  int required_allocated_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; // divide and round up
  int max_allocated_pages = KERNEL_SPACE_SIZE / PAGE_SIZE;
  if (required_allocated_pages > (max_allocated_pages - kernel->allocated_pages))
    return -1;

  // ! 1.3 Alloc space for page_table (the size of it depends on how many pages you need) and update allocated_pages.
  struct PageTable *page_table = (struct PageTable *)malloc(sizeof(struct PageTable));
  page_table->ptes = (struct PTE *)malloc(sizeof(struct PTE) * required_allocated_pages);
  kernel->allocated_pages += required_allocated_pages;
  kernel->mm[pid].size = required_allocated_pages * PAGE_SIZE;
  kernel->mm[pid].page_table = page_table;

  // ! 1.4 The mapping to kernel-managed memory is not built up, all the PFN should be set to -1 and present byte to 0.
  for (int i = 0; i < required_allocated_pages; i++)
  {
    page_table->ptes[i].PFN = -1;
    page_table->ptes[i].present = 0;
  }
  kernel->running[pid] = 1;

  // ! 1.5 Return the pid if success, -1 if failure.
  return pid;
}

/*
  This function will read the range [addr, addr+size) from user space of a specific process to the buf (buf should be >= size).
  1. Check if the reading range is out-of-bounds.
  2. If the pages in the range [addr, addr+size) of the user space of that process are not present,
     you should firstly map them to the free kernel-managed memory pages (first fit policy).
  Return 0 when success, -1 when failure (out of bounds).
*/
int vm_read(struct Kernel *kernel, int pid, char *addr, int size, char *buf)
{
  /* Fill Your Code Below */

  // ! 2.1 Check if the reading range is out-of-bounds.
  uintptr_t base = (uintptr_t)addr;
  int mm_size = kernel->mm[pid].size;
  int bound = base + size - 1;
  int start = base / PAGE_SIZE;
  int end = bound / PAGE_SIZE;
  int max_page = VIRTUAL_SPACE_SIZE / PAGE_SIZE;
  if (base < 0 || start < 0 || end < 0 || end < start || bound >= mm_size || start >= max_page || end >= max_page)
    return -1;

  // ! 2.2 first fit policy (scan from the beginning) if process are not present
  for (int i = start; i <= end; i++)
  {
    if (kernel->mm[pid].page_table->ptes[i].present == 0)
    {
      int free_page = 0;
      for (int j = 0; j < max_page; j++)
      {
        if (kernel->occupied_pages[j] == 0)
        {
          kernel->occupied_pages[j] = 1;
          kernel->mm[pid].page_table->ptes[i].PFN = j;
          kernel->mm[pid].page_table->ptes[i].present = 1;
          free_page = 1;
          break;
        }
      }
      if (!free_page)
        return -1;
    }
  }

  // ! read from user space to the (buf should be >= size)
  for (int i = 0; i < size; i++)
  {
    buf[i] = kernel->space[base + start * PAGE_SIZE + i];
  }

  // ! Return 0 when success, -1 when failure (out of bounds).
  return 0;
}

/*
  This function will write the content of buf to user space [addr, addr+size) (buf should be >= size).
  1. Check if the writing range is out-of-bounds.
  2. If the pages in the range [addr, addr+size) of the user space of that process are not present,
     you should firstly map them to the free kernel-managed memory pages (first fit policy).
  Return 0 when success, -1 when failure (out of bounds).
*/
int vm_write(struct Kernel *kernel, int pid, char *addr, int size, char *buf)
{
  /* Fill Your Code Below */

  // ! Check if the reading range is out-of-bounds.
  uintptr_t base = (uintptr_t)addr;
  int mm_size = kernel->mm[pid].size;
  int bound = base + size - 1;
  int start = base / PAGE_SIZE;
  int end = bound / PAGE_SIZE;
  int max_page = VIRTUAL_SPACE_SIZE / PAGE_SIZE;
  if (base < 0 || start < 0 || end < 0 || end < start || bound >= mm_size || start >= max_page || end >= max_page)
    return -1;

  // ! first fit policy (scan from the beginning) if process are not present
  for (int i = start; i <= end; i++)
  {
    if (kernel->mm[pid].page_table->ptes[i].present == 0)
    {
      int free_page = 0;
      for (int j = 0; j < max_page; j++)
      {
        if (kernel->occupied_pages[j] == 0)
        {
          kernel->occupied_pages[j] = 1;
          kernel->mm[pid].page_table->ptes[i].PFN = j;
          kernel->mm[pid].page_table->ptes[i].present = 1;
          free_page = 1;
          break;
        }
      }
      if (!free_page)
        return -1;
    }
  }

  // ! write the content of buf to user space
  for (int i = 0; i < size; i++)
  {
    kernel->space[base + start * PAGE_SIZE + i] = buf[i];
  }

  // ! Return 0 when success, -1 when failure (out of bounds).
  return 0;
}

/*
  This function will free the space of a process.
  1. Reset the corresponding pages in occupied_pages to 0.
  2. Release the page_table in the corresponding MMStruct and set to NULL.
  Return 0 when success, -1 when failure.
*/
int proc_exit_vm(struct Kernel *kernel, int pid)
{
  /* Fill Your Code Below */

  // ! 4.1 Reset the corresponding pages in occupied_pages to 0.
  int allocated_pages = kernel->mm[pid].size / PAGE_SIZE;
  for (int i = 0; i < allocated_pages; i++)
    kernel->occupied_pages[kernel->mm[pid].page_table->ptes[i].PFN] = 0;

  // ! 4.2 Release the page_table in the corresponding MMStruct and set to NULL.
  free(kernel->mm[pid].page_table->ptes);
  kernel->mm[pid].page_table->ptes = NULL;

  free(kernel->mm[pid].page_table);
  kernel->mm[pid].page_table = NULL;

  // ! Return 0 when success, -1 when failure.
  kernel->running[pid] = 0;
  return 0;
}
