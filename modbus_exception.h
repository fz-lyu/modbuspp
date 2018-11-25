//
// Created by Fanzhe on 5/29/2017.
//

#ifndef MODBUSPP_MODBUS_EXCEPTION_H
#define MODBUSPP_MODBUS_EXCEPTION_H

#include <exception>
using namespace std;


/// Modbus Exception Class
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


/// Modbus Connect Exception
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


/// Modbus Illgal Function Exception
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


/// Modbus Illegal Address Exception
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


/// Modbus Illegal Data Value Exception
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


/// Modbus Server Failure Exception
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


/// Modbus Acknowledge Exception
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


/// Modbus Server Busy Exception
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

/// Modbus Gate Way Problem Exception
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

/// Modbus Buffer Exception
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


/// Modbus Amount Exception
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
