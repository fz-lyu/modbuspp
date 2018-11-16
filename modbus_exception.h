//
// Created by Fanzhe on 5/29/2017.
//

#ifndef MODBUSPP_MODBUS_EXCEPTION_H
#define MODBUSPP_MODBUS_EXCEPTION_H

#include <exception>
using namespace std;


/**
 * Modbus Exeception Super Class
 *
 * Throwed when a exception or errors happens in modbus protocol
 */
class modbus_exception : public exception {
public:
    string msg;
    virtual const char* what() const throw()
    {
        return "A Error In Modbus Happened!";
    }
};


/**
 * Connection Issue
 *
 * Throwed when a connection issues happens between modbus client and server
 */
class modbus_connect_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Having Modbus Connection Problem";
    }
};


/**
 * Illegal Function
 *
 * Throwed when modbus server return error response function 0x01
 */
class modbus_illegal_function_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Illegal Function";
    }
};


/**
 * Illegal Address
 *
 * Throwed when modbus server return error response function 0x02
 */
class modbus_illegal_address_exception: public modbus_exception {
public:
    string msg = "test";
    const char* what() const throw()
    {
        return "Illegal Address";
    }
};


/**
 * Illegal Data Vlaue
 *
 * Throwed when modbus server return error response function 0x03
 */
class modbus_illegal_data_value_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Illegal Data Value";
    }
};


/**
 * Server Failure
 *
 * Throwed when modbus server return error response function 0x04
 */
class modbus_server_failure_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Server Failure";
    }
};


/**
 * Acknowledge
 *
 * Throwed when modbus server return error response function 0x05
 */
class modbus_acknowledge_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Acknowledge";
    }
};



/**
 * Server Busy
 *
 * Throwed when modbus server return error response function 0x06
 */
class modbus_server_busy_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Server Busy";
    }
};


/**
 * Gate Way Problem
 *
 * Throwed when modbus server return error response function 0x0A and 0x0B
 */
class modbus_gateway_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Gateway Problem";
    }
};


/**
 * Buffer Exception
 *
 * Throwed when buffer is too small for the data to be storaged.
 */
class modbus_buffer_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Size of Buffer Is too Small!";
    }
};


/**
 * Amount Exception
 *
 * Throwed when the address or amount input is mismatching.
 */
class modbus_amount_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Too many Data!";
    }
};



#endif //MODBUSPP_MODBUS_EXCEPTION_H
