//
// Created by Fanzhe on 5/29/2017.
//

#ifndef MODBUSPP_MODBUS_EXCEPTION_H
#define MODBUSPP_MODBUS_EXCEPTION_H

#include <exception>
using namespace std;


/**
 * MODBS EXCPETION
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
 * Represent Connection Issue in Class
 */
class modbus_connect_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Having Modbus Connection Problem";
    }
};


/**
 * Illegal Function      Error response Function 0x01
 */
class modbus_illegal_function_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Illegal Function";
    }
};


/**
 * Illegal Address      Error Response Function 0x02
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
 * Illegal Data Vlaue   Error Response Funciton 0x03
 */
class modbus_illegal_data_value_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Illegal Data Value";
    }
};


/**
 * Server Failure       Error Response Function 0x04
 */
class modbus_server_failure_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Server Failure";
    }
};


/**
 * Acknowledge          Error Response Function 0x05
 */
class modbus_acknowledge_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Acknowledge";
    }
};



/**
 * Server Busy           Error Response Function 0x06
 */
class modbus_server_busy_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Server Busy";
    }
};


/**
 * Gate Way Problem     Error Response Function 0x0A 0x0B
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
 * Buffer is Too Small for Data Storage
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
 * Address or Amount Input is Wrong
 */
class modbus_amount_exception: public modbus_exception {
public:
    virtual const char* what() const throw()
    {
        return "Too many Data!";
    }
};



#endif //MODBUSPP_MODBUS_EXCEPTION_H
