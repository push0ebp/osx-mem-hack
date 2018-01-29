# OS X Memory hack
 xnu memory manipulation library

## Features
* Read memory
* Write memory
* get image base address
* get pid

you must to open process with `Open(<process name>)`. 
if you do not, then causes throw exception.


## Example
```c++
Process *process = new Process();
process->Open("Safari");
uint64_t base = process->get_base_address();
pid_t pid = process->get_pid();
cout << "pid : " << pid << endl;
cout << "base address : 0x" << hex << base << endl;
cout << "magic : 0x" << hex << process->Read<uint32_t>(base) << endl;
process->Write<uint32_t>(base, 0xdeadbeef);
cout << "written magic : 0x" << hex << process->Read<uint32_t>(base) << endl;
```

#### mem.cpp
```
# ./mem Safari
process : Safari
pid : 86488
opened process.
base address : 0x102717000
read/write to 0x102717000
magic 0xfeedfacf
write 0xdeadbeef
magic 0xdeadbeef
read/write to 0x102717028
read __PAGEZERO
write "__PUSH0EBP"
read __PUSH0EBP
```

## Functions
### Open Process
#### type
```c++ 
kern_return_t Open(char *process_name)`
```
#### example
```c++
process->Open("Safari”);
````
if pid is valid and open fail, you are not root.
if pid is not valid (-1) and open fail, process was not found.
  
### Read Memory
#### type
```c++
vm_size_t Read(uint64_t addr, void *data, vm_size_t size)
```
for bytes. return read bytes size

```c++
template <typename T> T Read(uint64_t addr)
``` 
for value

#### example
```c++
process->Read<uint32_t>(base); //0xfeedfacf
process->Read(base + 0x28, buffer, sizeof(buffer)); //__PAGEZERO
```

### Write Memory
#### type 
```c++
kern_return_t Write(uint64_t addr, void *data, vm_size_t size)
```
for bytes.

```c++
template <typename T> T Read(uint64_t addr)
```
for value.

#### example
```c++
process->Write<uint32_t>(base, magic_value); 
process->Write(addr, value, sizeof(value));
```

#### Get Base Address
```c++
process->get_base_address()
```

#### Get Process id
```c++
process->get_pid()
```

