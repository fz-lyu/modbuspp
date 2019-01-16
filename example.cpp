// ModbusPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//
// Created by Fanzhe on 5/29/2017.
//

#include "../modbuspp-master/modbus.h"
#include "../modbuspp-master/modbus_exception.h"


int main(int argc, char **argv)
{
	// create a modbus object
	modbus mb = modbus("127.0.0.1", 502);


	// set slave id
	mb.modbus_set_slave_id(1);

	// connect with the server
	mb.modbus_connect();

	// read coil                        function 0x01
	bool read_coil;
	mb.modbus_read_coils(0, 1, &read_coil);


	// read input bits(discrete input)  function 0x02
	bool read_bits;
	mb.modbus_read_input_bits(0, 1, &read_bits);


	// read holding registers           function 0x03
	uint16_t read_holding_regs[1];
	mb.modbus_read_holding_registers(0, 1, read_holding_regs);



	// read input registers             function 0x04
	uint16_t read_input_regs[1];
	mb.modbus_read_input_registers(0, 1, read_input_regs);


	// write single coil                function 0x05
	mb.modbus_write_coil(0, true);



	// write single reg                 function 0x06
	mb.modbus_write_register(0, 123);


	// write multiple coils             function 0x0F
	bool write_col[1] = { true };
	mb.modbus_write_coils(8, 1, write_col);

	bool write_cols[10] = { true, true, true, true,true, true, true, true,true, true };
	mb.modbus_write_coils(0, 4, write_cols);


	// write multiple regs              function 0x10
	uint16_t write_regs[4] = { 123, 123, 123,125 };
	mb.modbus_write_registers(0, 4, write_regs);


	// close connection and free the memory
	mb.modbus_close();
	delete(&mb);
	return 0;
}










//#include "pch.h"
//#include <iostream>

//int main()
//{
//    std::cout << "Hello World!\n"; 
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
