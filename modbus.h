//
// Created by Fanzhe on 5/28/2017.
//

#ifndef MODBUSPP_MODBUS_H
#define MODBUSPP_MODBUS_H

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "modbus_exception.h"
using namespace std;


#define MAX_MSG_LENGTH 260

///Function Code
enum{
    READ_COILS       = 0x01,
    READ_INPUT_BITS  = 0x02,
    READ_REGS        = 0x03,
    READ_INPUT_REGS  = 0x04,
    WRITE_COIL       = 0x05,
    WRITE_REG        = 0x06,
    WRITE_COILS      = 0x0F,
    WRITE_REGS       = 0x10,
};

///Exception Codes
enum {
    EX_ILLEGAL_FUNCTION = 0x01, // Function Code not Supported
    EX_ILLEGAL_ADDRESS  = 0x02, // Output Address not exists
    EX_ILLEGAL_VALUE    = 0x03, // Output Value not in Range
    EX_SERVER_FAILURE   = 0x04, // Slave Deive Fails to process request
    EX_ACKNOWLEDGE      = 0x05, // Service Need Long Time to Execute
    EX_SERVER_BUSY      = 0x06, // Server Was Unable to Accept MB Request PDU
    EX_GATEWAY_PROBLEMP = 0x0A, // Gateway Path not Available
    EX_GATEWYA_PROBLEMF = 0x0B, // Target Device Failed to Response
};


/// Modbus Operator Class
/**
 * Modbus Operator Class
 * Providing networking support and mobus operation support.
 */
class modbus {
private:
    bool _connected;
    uint16_t PORT;
    int _socket;
    int _msg_id;
    int _slaveid ;
    string HOST;
    struct sockaddr_in _server;

    void modbus_build_request(uint8_t *to_send,int address, int func);

    void modbus_read(int address, int amount, int func);
    void modbus_write(int address, int amount, int func, uint16_t *value);

    ssize_t modbus_send(uint8_t *to_send, int length);
    ssize_t modbus_receive(uint8_t *buffer);

    void modbus_error_handle(uint8_t *msg, int func);


public:
    modbus(string host, uint16_t port);
    modbus(string host);
    ~modbus();

    bool modbus_connect();
    void modbus_close();

    void modbus_set_slave_id(int id);

    void modbus_read_coils(int address, int amount, bool* buffer);
    void modbus_read_input_bits(int address, int amount, bool* buffer);
    void modbus_read_holding_registers(int address, int amount, uint16_t *buffer);
    void modbus_read_input_registers(int address, int amount, uint16_t *buffer);

    void modbus_write_coil(int address, bool to_write);
    void modbus_write_register(int address, uint16_t value);
    void modbus_write_coils(int address, int amount, bool* value );
    void modbus_write_registers(int address, int amount, uint16_t *value);
};


#endif //MODBUSPP_MODBUS_H
