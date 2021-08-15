/* example.cpp
 *
 * Copyright (C) 20017-2021 Fanzhe Lyu <lvfanzhe@hotmail.com>, all rights reserved.
 *
 * modbuspp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "modbus.h"

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
    bool write_cols[4] = {true, true, true, true};
    mb.modbus_write_coils(0, 4, write_cols);

    // write multiple regs              function 0x10
    uint16_t write_regs[4] = {123, 123, 123};
    mb.modbus_write_registers(0, 4, write_regs);

    // close connection and free the memory
    mb.modbus_close();
    return 0;
}
