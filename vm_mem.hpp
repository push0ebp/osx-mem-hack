#ifndef _VM_MEM_H_
#define _VM_MEM_H_

#include <sys/sysctl.h>

#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_vm.h>

#include <cassert>

int align_size(size_t size);

class Process
{
 public:    
  Process() : pid_(-1), imagebase_(0) {};

  kern_return_t Open(char *process_name);

  vm_size_t Read(uint64_t addr, void *data, vm_size_t size);
  template <typename T> T Read(uint64_t addr)
  {
    T value;
    Read(addr, &value, sizeof(value));
    return value;
  }
  
  
  kern_return_t Write(uint64_t addr, void *data, vm_size_t size);
  template<typename T> kern_return_t Write(uint64_t addr, T value)
  {
      if(Write(addr, &value, sizeof(value)) == KERN_SUCCESS)
        return sizeof(value);
  }
  uint64_t get_base_address() { return imagebase_; }
  pid_t get_pid() { return pid_; }
  
 private:
  pid_t pid_;
  mach_port_t task_;
  int is_opened_;
  uint64_t imagebase_;
  const char *kProcessExceptionMessage_ = "Process was not opened";
  pid_t GetPid(char *process_name);
  uint64_t GetBaseAddress();
  
};
#endif