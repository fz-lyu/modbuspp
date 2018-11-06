# MODBUS++  [(MODBSPP)](https://github.com/fanzhe98/modbuspp.git)   V0.1
MODBUS++ (MODBUSPP) is an open source c++ class/ library making an encapsulation of [Modbus](https://en.wikipedia.org/wiki/Modbus) TCP Protocol published by Modicon (Now Schneider Electirc).

MODBUS++ is based on Object-Oriented Programming. While it keeps the efficiency of C++ Code , it provides a higher level of abstraction than other C Modbus Library. Generally, it is easier for programmers to use in their development requiring MODBUS TCP Protocol. Compared to other known modbus library, such as libmodbus, MODBUS++ is providing a more OOP friendly syntaxes.

The code has dependencies on libary on Linux for TCP/IP, if you want to use this in Windows, please check out winsock2 and rewrite portion of code of socket to be compatible with Windows operating system.

# Usage
To use the library, please follow the steps below. Please note current library is only compatible with Linux distributions because of the socket library dependencies.

## Download
Download the MODBUS++, you can:
- Open your command window (cmd, shell, bash, etc.)
> git clone https://github.com/fanzhe98/modbuspp.git
- Directly Download From the [Page](https://github.com/fanzhe98/modbuspp.git)

## Include In your code
To include Modbus++ in your code, just simply type this in the start of your program:
>  include "modbus.h"
>  include "modbus_exception.h"

# Getting Started with a Example
## Getting the Example
Please checkout example.cpp for a example usage of MODBUSPP. Please note that the code should be compiled using c++11 compilers.
## Tutorials
Include the header
> #include "modbus.h"
This line includes the header modbus.h from the library, this will tell the compiler to look for functions and variables in the related file.

Create and connects a mobus server
> modbus mb = modbus("127.0.0.1", 502);
> mb.modbus_set_slave_id(1);
> mb.modbus_connect();
These lines creates local modbus client with target IP address at 127.0.0.1 and port at 502, a server with id 1.

Read a coil (function 0x01)
> bool read_coil;
> mb.modbus_read_coils(0, 1, &read_coil);
These lines read a bit from the coil at address 0.

Read input bits(discrete input) (function 0x02)
> bool read_bits;
> mb.modbus_read_input_bits(0, 1, &read_bits);
These lines read a bit from discrete inputs at address 0.

Read holding registers (function 0x03)
> uint16_t read_holding_regs[1];
> mb.modbus_read_holding_registers(0, 1, read_holding_regs);
These lines read a word(16 bits) from holding registers starting at address 0.



Read input registers (function 0x04)
> uint16_t read_input_regs[1];
> mb.modbus_read_input_registers(0, 1, read_input_regs);
These lines read a word(16 bits) from input registers starting at address 0.

Write single coil (function 0x05)
> mb.modbus_write_coil(0, true);
These lines write a bit to the coil at address 0.



Write single reg (function 0x06)
> mb.modbus_write_register(0, 123);
These lines write a word(16 bits) to the register at address 0.


Write multiple coils (function 0x0F)
> bool write_cols[4] = {true, true, true, true};
> mb.modbus_write_coils(0,4,write_cols);
These lines write multiple bits to the coil starting from address 0, for a length of 4.


Write multiple regs (function 0x10)
> uint16_t write_regs[4] = {123, 123, 123};
> mb.modbus_write_registers(0, 4, write_regs);
These lines write multiple words to the register starting from address 0, for a length of 4.

