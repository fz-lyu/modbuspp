# MODBUS++  [(MODBSPP)](https://github.com/fanzhe98/modbuspp.git)   v0.3
[![CMake](https://github.com/fanzhe98/modbuspp/actions/workflows/cmake.yml/badge.svg)](https://github.com/fanzhe98/modbuspp/actions/workflows/cmake.yml)
# 0 Info, Updates and Discalimer
## 0.0 Info
MODBUS++ (MODBUSPP) is an header-only open source c++ class/ library making an encapsulation of [Modbus](https://en.wikipedia.org/wiki/Modbus) TCP Protocol for client.

MODBUS++ is providing a more OOP friendly syntax.

The code now is modified to extend for Windows! However, the code for running on Windows is not 
throughly tested.

## 0.1 Updates 

UPDATES in v0.3 compared to V0.2:
1. Bug Fixing
2. Type fixing
3. Initial Windows support (in dev, not fully tested)
4. Logging as Option
5. Cmake install script
6. Pasting licensing information in file

# 1 Usage
To use the library, please follow the steps below. Please note current library is only compatible with Linux distributions because of the socket library dependencies.

## 1.1 Download
Download the MODBUS++, you can:
   - Open your command window (cmd, shell, bash, etc.)
        > git clone https://github.com/fanzhe98/modbuspp.git
   - Directly download from the page.

## 1.2 Include In your code
1. Copy the "moodbus.h" to your include directory and include the code whenever appropriate:
   >  include "modbus.h"
2. On Linux platform, you could also use 
   ```sh
   mkdir build
   cd build
   cmake ..
   sudo make install
   ```
   This will install the header under usr/local/include
   and include the code whenever appropriate
   > include "modbuspp/modbus.h"

## 1.3 Logging as Option
To enable logging, simply set `ENABLE_MODBUSPP_LOGGING` in your compile option, see CMakeLists.txt with example.

# 2 Getting Started with a Example
## 2.1 Getting the Example
Checkout [example.cpp](https://github.com/fanzhe98/modbuspp/blob/master/example/example.cpp) for a example usage of MODBUSPP. Please note that the code should be compiled using c++11 compilers.
A sample CMakeLists.txt is included. To build example:
### 2.1.1 CMAKE 
```shell script
mkdir build
cd build
cmake ..
make
```
### 2.1.2 G++
```shell script
g++ example.cpp --std=c++11 -o example
```
### 2.1.3 MSCV
Open your Visual Studio to compile the project

## 2.2 Tutorials
Let's break the code down, before you read this, reference to [example.cpp](https://github.com/fanzhe98/modbuspp/blob/master/example.cpp). The example shows basic usages of how to create a modbus client connecting to a modbus server and perform modbus operations in your program.
### Include the header

To start with, be sure to include the modbus.h header in your program
> #include "modbus.h"

This line includes the header modbus.h from the library, this will tell the compiler to look for functions and variables in the related file.

### Create and connects a mobus server
Before performing any modbus operations, a connection needed to be setup between the modbus client(your program) and a modbus server(could be a controller).

> modbus mb = modbus("127.0.0.1", 502);
> mb.modbus_set_slave_id(1);
> mb.modbus_connect();

These lines creates local modbus client with target IP address at 127.0.0.1 and port at 502, a server with id 1.

### Performe Mobus Operations
Modbus usually have 2 types of operations, reading and writing. Reading operations include reading a coil, reading input bits, reading holding registers, and reading input registers. Writing Operations include writing sigle coil, wirting single register, writing multiple coil, and writing multiple registers. Be sure to create a connection before performing any operations. Modbus is relatively low-level protocols. It is useful to have strong computer structure knowledge to understand it.

1. The following line show how to read a coil (function 0x01). These lines read a bit from the coil at address 0.
> bool read_coil;
> mb.modbus_read_coils(0, 1, &read_coil);

2. The following lines show how to read input bits(discrete input) (function 0x02). These lines read a bit from discrete inputs at address 0.
> bool read_bits;
> mb.modbus_read_input_bits(0, 1, &read_bits);

3. The follwing lines show how to read holding registers (function 0x03). These lines read a word(16 bits) from holding registers starting at address 0.
> uint16_t read_holding_regs[1];
> mb.modbus_read_holding_registers(0, 1, read_holding_regs);

4. The following lines show how to Read input registers (function 0x04). These lines read a word(16 bits) from input registers starting at address 0.
> uint16_t read_input_regs[1];
> mb.modbus_read_input_registers(0, 1, read_input_regs);

5. The following lines show how to write single coil (function 0x05). These lines write a bit to the coil at address 0.
> mb.modbus_write_coil(0, true);

6. The following lines show how to write single register (function 0x06). These lines write a word(16 bits) to the register at address 0.
> mb.modbus_write_register(0, 123);

7. The following lines show how to write multiple coils (function 0x0F). These lines write multiple bits to the coil starting from address 0, for a length of 4.
> bool write_cols[4] = {true, true, true, true};
> mb.modbus_write_coils(0,4,write_cols);

8. The following lines show how to write multiple regs (function 0x10). These lines write multiple words to the register starting from address 0, for a length of 4.
> uint16_t write_regs[4] = {123, 123, 123};
> mb.modbus_write_registers(0, 4, write_regs);

