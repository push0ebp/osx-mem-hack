#ifndef _VM_MEM_H_
#define _VM_MEM_H_

#include <sys/sysctl.h>

#include <mach/mach.h>

typedef struct kinfo_proc kinfo_proc;

int align_size(size_t size);

uint64_t get_base_address(mach_port_t task);
pid_t get_pid(char *proc_name);
kern_return_t attach(pid_t pid, mach_port_t *task);

kern_return_t mem_write(mach_port_t task, uint64_t addr, void *data, int size);
kern_return_t mem_read(mach_port_t task, uint64_t addr, void *data, int size);


#endif