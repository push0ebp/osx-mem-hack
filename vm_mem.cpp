#include "vm_mem.hpp"

#include <memory>

int align_size(size_t size)
{
  int align = sizeof(long);
  int aligned_size =  (size / align) * align + align;
  return aligned_size;
}


uint64_t Process::GetBaseAddress()
{
  kern_return_t kret;
  
  vm_map_size_t vm_size = 0;
  mach_vm_address_t base_addr = 0;
  natural_t depth = 0;
  struct vm_region_submap_info_64 region_info;
  mach_msg_type_number_t region_info_cnt = sizeof(region_info);

  kret = mach_vm_region_recurse(task_, &base_addr, &vm_size,
                                &depth, (vm_region_recurse_info_t)&region_info, &region_info_cnt); 
  if(kret != KERN_SUCCESS)
  {
    return 0;
  }

  return base_addr;
}

pid_t Process::GetPid(char *process_name)
{
  int err = 0;
  size_t length = 0;
  
  int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

  err = sysctl(mib, (sizeof(mib) / sizeof(*mib)) - 1, NULL, &length, NULL, 0);
  if(err) 
  {
    return -1;
  }
  
  int proc_cnt = length / sizeof(kinfo_proc);
  std::unique_ptr<kinfo_proc[]> procs(new kinfo_proc[proc_cnt]);

  err = sysctl(mib, (sizeof(mib) / sizeof(*mib)) - 1, procs.get(), &length, NULL, 0);
  if(err) 
  {
    return -1;
  }
  
  for (int i = 0; i < proc_cnt; i++) 
  {
    if(!strcmp(procs[i].kp_proc.p_comm, process_name))
    {
      return procs[i].kp_proc.p_pid;
    }
  }
  return -1;
}


kern_return_t Process::Open(char *process_name)
{
  pid_ = GetPid(process_name);
  if(pid_ == -1)
    return KERN_FAILURE;   
  
  kern_return_t kret;
  kret = task_for_pid(mach_task_self(), pid_, &task_);

  if ((kret != KERN_SUCCESS) || !MACH_PORT_VALID(task_)) 
  {
    return kret;
  }
  
  imagebase_ = GetBaseAddress();
  if(imagebase_ == 0)
  {
      return KERN_FAILURE;
  }
  is_opened_ = 1;
  return kret;
}

kern_return_t Process::Write(uint64_t addr, void *data, vm_size_t size)
{
  if(!is_opened_)
    throw kProcessExceptionMessage_;
    
  kern_return_t kret;
  vm_size_t vm_size = align_size(size);
  kret = vm_protect(task_, (vm_address_t)addr, (vm_size_t)vm_size, 0, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_ALL);
  if(kret != KERN_SUCCESS)
  {
    return kret;
  }
  
  kret = vm_write(task_, (vm_address_t) addr, (vm_address_t)data, size); 
  if(kret != KERN_SUCCESS)
  {
    return kret;
  }
  return kret; 
}

vm_size_t Process::Read(uint64_t addr, void *data, vm_size_t size)
{
  if(!is_opened_)
    throw kProcessExceptionMessage_;
    
  kern_return_t kret;
  vm_size_t read_size = 0;
  kret = vm_read_overwrite(task_, (vm_address_t) addr, size, (vm_address_t)data, &read_size); 
  if(kret != KERN_SUCCESS)
  {
    return 0;
  }
  return read_size; 
}
