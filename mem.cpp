#include "vm_mem.hpp"

#include <iostream>
#include <memory>
#include <unistd.h>

using namespace std;

int main(int argc, char *argv[])
{
  const int offset = 0x28;
  if(argc < 2)
  {
    cout << "Usage : ./mem <process name>" << endl;
    exit(-1);
  }
  if(getuid() && geteuid()) 
  {
    cout << "permission denied, please root." << endl;
    exit(-1); //
  } 

  cout << "process : " << argv[1] << endl;
  Process *process = new Process();
  kern_return_t kret = process->Open(argv[1]);

  pid_t pid = process->get_pid();
  cout << "pid : " << pid << endl;

  if(pid != -1)
  {
    if(kret != KERN_SUCCESS)
    {
        cout << "open error, please check root." << endl;
    }
    else
    {
      cout << "opened process." << endl;
    }
  }
  else
  {
    cout << "process not found." << endl;
    exit(-1);
  }

  uint64_t base = process->get_base_address();
  cout << "base address : 0x" << hex << base << endl;
  
  char buffer[11] = {0};
  char value[11] = "__PUSH0EBP";
  uint64_t addr = base + offset;

  cout << "read/write to 0x" << hex << base << endl;
  uint32_t magic = process->Read<uint32_t>(base);
  uint32_t magic_value = 0xdeadbeef;
  cout << "magic 0x" << hex << magic << endl;
  cout << "write 0x" << magic_value << endl;
  process->Write<uint32_t>(base, magic_value);
  magic = process->Read<uint32_t>(base);
  cout << "magic 0x" << hex << magic << endl; 

  cout << "read/write to 0x" << hex << addr << endl;
  process->Read(addr, buffer, sizeof(buffer));
  cout << "read " << buffer << endl; 

  process->Write(addr, value, sizeof(value));
  cout << "write \"" << value << "\"" << endl;
  memset(buffer, 0, sizeof(buffer));
  process->Read( addr, buffer, sizeof(buffer));
  cout << "read " << buffer << endl; 

  return 0;
}



