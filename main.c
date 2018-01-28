#include <stdio.h>
#include "vm_mem.h"


int main()
{
  const int offset = 0x28;

  if(getuid() && geteuid()) 
  {
    printf("permission denied, please root\n");
  } 
  pid_t pid = get_pid("Safari");
  if(pid == -1)
  {
    printf("process not found\n");
    exit(-1);
  }
  printf("pid : %d\n", pid);
  
  mach_port_t task;
  attach(pid, &task);

  uint64_t base = get_base_address(pid, task);
  printf("base address : %#llx\n",base);
  
  char buffer[11] = {0};
  char value[11] = "__PUSH0EBP";
  uint64_t addr = base + offset;
  printf("read/write to %#llx\n",addr);
  mem_read(task, addr, buffer, sizeof(buffer));
  printf("read %s\n", buffer); 

  mem_write(task, addr, value, sizeof(value));
  printf("write \"%s\"\n", value);
  memset(buffer, 0, sizeof(buffer));
  mem_read(task, addr, buffer, sizeof(buffer));
  printf("read %s\n", buffer); 
return 0;
}



