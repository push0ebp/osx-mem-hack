#include "vm_mem.h"

int align_size(size_t size)
{
  int align = sizeof(long);
  int aligned_size =  (size / align) * align + align;
  return aligned_size;
}


uint64_t get_base_address(mach_port_t task)
{
  kern_return_t kret;
  
  vm_map_size_t vm_size = 0;
  mach_vm_address_t base_addr = 0;
  natural_t depth = 0;
  struct vm_region_submap_info_64 region_info;
  mach_msg_type_number_t region_info_cnt = sizeof(region_info);

  kret = mach_vm_region_recurse(task, &base_addr, &vm_size,
                                &depth, (vm_region_recurse_info_t)&region_info, &region_info_cnt); 
  if(kret != KERN_SUCCESS)
  {
    printf("mach_vm_region_recurse() error %s\n", mach_error_string(kret));
    return kret;
  }

  return base_addr;
}

pid_t get_pid(char *proc_name)
{

  kinfo_proc *procs = NULL;
  int err = 0;
  size_t length = 0;
  
  int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

  err = sysctl(mib, (sizeof(mib) / sizeof(*mib)) - 1, NULL, &length, NULL, 0);
  if(err) 
    return err;
  
  procs = (kinfo_proc*)malloc(length);

  err = sysctl(mib, (sizeof(mib) / sizeof(*mib)) - 1, procs, &length, NULL, 0);
  if(err) 
  {
    perror("sysctl() error ");
    free(procs);
    return err;
  }
  
  int proc_cnt = length / sizeof(kinfo_proc);
  for (int i = 0; i < proc_cnt; i++) 
  {
    if(!strcmp(procs[i].kp_proc.p_comm, proc_name))
    {
      free(procs);
      return procs[i].kp_proc.p_pid;
    }
  }
  free(procs);
  return -1;
}


kern_return_t attach(pid_t pid, mach_port_t *task)
{
  kern_return_t kret;
  kret = task_for_pid(mach_task_self(), pid, task);

  if ((kret != KERN_SUCCESS) || !MACH_PORT_VALID(*task)) 
  {
    printf("task_for_pid() error %s\n", mach_error_string(kret));
    return kret;
  }

  return kret;
}

kern_return_t mem_write(mach_port_t task, uint64_t addr, void *data, int size)
{
  kern_return_t kret;
  vm_size_t vm_size = align_size(size);
  kret = vm_protect(task, (vm_address_t)addr, (vm_size_t)vm_size, 0, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_ALL);
  if(kret != KERN_SUCCESS)
  {
    printf("vm_protect() error %s\n", mach_error_string(kret));
    return kret;
  }
  
  kret = vm_write(task, (vm_address_t) addr, (vm_address_t)data, size); 
  if(kret != KERN_SUCCESS)
  {
    printf("vm_write() error %s\n", mach_error_string(kret));
    return kret;
  }
  return kret; 
}

kern_return_t mem_read(mach_port_t task, uint64_t addr, void *data, int size)
{
  kern_return_t kret;
  vm_size_t read_size = 0;
  kret = vm_read_overwrite(task, (vm_address_t) addr, size, (vm_address_t)data, &read_size); 
  if(kret != KERN_SUCCESS)
  {
    printf("vm_read_overwrite() error %s\n", mach_error_string(kret));
    return kret;
  }
  return kret; 
}
