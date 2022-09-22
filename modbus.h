/* modbus.h
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

#ifndef MODBUSPP_MODBUS_H
#define MODBUSPP_MODBUS_H

#include <cstring>
#include <stdint.h>
#include <string>

#ifdef ENABLE_MODBUSPP_LOGGING
#include <cstdio>
#define LOG(fmt, ...) printf("[ modbuspp ]" fmt, ##__VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

#ifdef _WIN32
// WINDOWS socket
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
using X_SOCKET = SOCKET;
using ssize_t = int;

#define X_ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define X_CLOSE_SOCKET(s) closesocket(s)
#define X_ISCONNECTSUCCEED(s) ((s) != SOCKET_ERROR)

#else
// Berkeley socket
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using X_SOCKET = int;

#define X_ISVALIDSOCKET(s) ((s) >= 0)
#define X_CLOSE_SOCKET(s) close(s)
#define X_ISCONNECTSUCCEED(s) ((s) >= 0)
#endif

using SOCKADDR = struct sockaddr;
using SOCKADDR_IN = struct sockaddr_in;

#define MAX_MSG_LENGTH 260

///Function Code
#define READ_COILS 0x01
#define READ_INPUT_BITS 0x02
#define READ_REGS 0x03
#define READ_INPUT_REGS 0x04
#define WRITE_COIL 0x05
#define WRITE_REG 0x06
#define WRITE_COILS 0x0F
#define WRITE_REGS 0x10

///Exception Codes

#define EX_ILLEGAL_FUNCTION 0x01 // Function Code not Supported
#define EX_ILLEGAL_ADDRESS 0x02  // Output Address not exists
#define EX_ILLEGAL_VALUE 0x03    // Output Value not in Range
#define EX_SERVER_FAILURE 0x04   // Slave Deive Fails to process request
#define EX_ACKNOWLEDGE 0x05      // Service Need Long Time to Execute
#define EX_SERVER_BUSY 0x06      // Server Was Unable to Accept MB Request PDU
#define EX_NEGATIVE_ACK 0x07
#define EX_MEM_PARITY_PROB 0x08
#define EX_GATEWAY_PROBLEMP 0x0A // Gateway Path not Available
#define EX_GATEWAY_PROBLEMF 0x0B // Target Device Failed to Response
#define EX_BAD_DATA 0XFF         // Bad Data lenght or Address

#define BAD_CON -1

/// Modbus Operator Class
/**
 * Modbus Operator Class
 * Providing networking support and mobus operation support.
 */
class modbus
{

public:
    bool err{};
    int err_no{};
    std::string error_msg;

    modbus(std::string host, uint16_t port);
    ~modbus();

    bool modbus_connect();
    void modbus_close() const;

    bool is_connected() const { return _connected; }

    void modbus_set_slave_id(int id);

    int modbus_read_coils(uint16_t address, uint16_t amount, bool *buffer);
    int modbus_read_input_bits(uint16_t address, uint16_t amount, bool *buffer);
    int modbus_read_holding_registers(uint16_t address, uint16_t amount, uint16_t *buffer);
    int modbus_read_input_registers(uint16_t address, uint16_t amount, uint16_t *buffer);

    int modbus_write_coil(uint16_t address, const bool &to_write);
    int modbus_write_register(uint16_t address, const uint16_t &value);
    int modbus_write_coils(uint16_t address, uint16_t amount, const bool *value);
    int modbus_write_registers(uint16_t address, uint16_t amount, const uint16_t *value);

private:
    bool _connected{};
    uint16_t PORT{};
    uint32_t _msg_id{};
    int _slaveid{};
    std::string HOST;

    X_SOCKET _socket{};
    SOCKADDR_IN _server{};

#ifdef _WIN32
    WSADATA wsadata;
#endif

    void modbus_build_request(uint8_t *to_send, uint16_t address, int func) const;

    int modbus_read(uint16_t address, uint16_t amount, int func);
    int modbus_write(uint16_t address, uint16_t amount, int func, const uint16_t *value);

    ssize_t modbus_send(uint8_t *to_send, size_t length);
    ssize_t modbus_receive(uint8_t *buffer) const;

    void modbuserror_handle(const uint8_t *msg, int func);

    void set_bad_con();
    void set_bad_input();
};

/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */
inline modbus::modbus(std::string host, uint16_t port = 502)
{
    HOST = host;
    PORT = port;
    _slaveid = 1;
    _msg_id = 1;
    _connected = false;
    err = false;
    err_no = 0;
    error_msg = "";
}

/**
 * Destructor of Modbus Connector Object
 */
inline modbus::~modbus(void) = default;

/**
 * Modbus Slave ID Setter
 * @param id  ID of the Modbus Server Slave
 */
inline void modbus::modbus_set_slave_id(int id)
{
    _slaveid = id;
}

/**
 * Build up a Modbus/TCP Connection
 * @return   If A Connection Is Successfully Built
 */
inline bool modbus::modbus_connect()
{
    if (HOST.empty() || PORT == 0)
    {
        LOG("Missing Host and Port");
        return false;
    }
    else
    {
        LOG("Found Proper Host %s and Port %d", HOST.c_str(), PORT);
    }

#ifdef _WIN32
    if (WSAStartup(0x0202, &wsadata))
    {
        return false;
    }
#endif

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (!X_ISVALIDSOCKET(_socket))
    {
        LOG("Error Opening Socket");
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }
    else
    {
        LOG("Socket Opened Successfully");
    }

#ifdef WIN32
    const DWORD timeout = 20;
#else
    struct timeval timeout
    {
    };
    timeout.tv_sec = 20; // after 20 seconds connect() will timeout
    timeout.tv_usec = 0;
#endif

    setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = inet_addr(HOST.c_str());
    _server.sin_port = htons(PORT);

    if (!X_ISCONNECTSUCCEED(connect(_socket, (SOCKADDR *)&_server, sizeof(_server))))
    {
        LOG("Connection Error");
#ifdef _WIN32
        WSACleanup();
#endif
        return false;
    }

    LOG("Connected");
    _connected = true;
    return true;
}

/**
 * Close the Modbus/TCP Connection
 */
inline void modbus::modbus_close() const
{
    X_CLOSE_SOCKET(_socket);
#ifdef _WIN32
    WSACleanup();
#endif
    LOG("Socket Closed");
}

/**
 * Modbus Request Builder
 * @param to_send   Message Buffer to Be Sent
 * @param address   Reference Address
 * @param func      Modbus Functional Code
 */
inline void modbus::modbus_build_request(uint8_t *to_send, uint16_t address, int func) const
{
    to_send[0] = (uint8_t)(_msg_id >> 8u);
    to_send[1] = (uint8_t)(_msg_id & 0x00FFu);
    to_send[2] = 0;
    to_send[3] = 0;
    to_send[4] = 0;
    to_send[6] = (uint8_t)_slaveid;
    to_send[7] = (uint8_t)func;
    to_send[8] = (uint8_t)(address >> 8u);
    to_send[9] = (uint8_t)(address & 0x00FFu);
}

/**
 * Write Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of data to be Written
 * @param func      Modbus Functional Code
 * @param value     Data to Be Written
 */
inline int modbus::modbus_write(uint16_t address, uint16_t amount, int func, const uint16_t *value)
{
    int status = 0;
    uint8_t *to_send;
    if (func == WRITE_COIL || func == WRITE_REG)
    {
        to_send = new uint8_t[12];
        modbus_build_request(to_send, address, func);
        to_send[5] = 6;
        to_send[10] = (uint8_t)(value[0] >> 8u);
        to_send[11] = (uint8_t)(value[0] & 0x00FFu);
        status = modbus_send(to_send, 12);
    }
    else if (func == WRITE_REGS)
    {
        to_send = new uint8_t[13 + 2 * amount];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t)(7 + 2 * amount);
        to_send[10] = (uint8_t)(amount >> 8u);
        to_send[11] = (uint8_t)(amount & 0x00FFu);
        to_send[12] = (uint8_t)(2 * amount);
        for (int i = 0; i < amount; i++)
        {
            to_send[13 + 2 * i] = (uint8_t)(value[i] >> 8u);
            to_send[14 + 2 * i] = (uint8_t)(value[i] & 0x00FFu);
        }
        status = modbus_send(to_send, 13 + 2 * amount);
    }
    else if (func == WRITE_COILS)
    {
        to_send = new uint8_t[14 + (amount - 1) / 8];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t)(7 + (amount + 7) / 8);
        to_send[10] = (uint8_t)(amount >> 8u);
        to_send[11] = (uint8_t)(amount & 0x00FFu);
        to_send[12] = (uint8_t)((amount + 7) / 8);
        for (int i = 0; i < (amount + 7) / 8; i++)
            to_send[13 + i] = 0; // init needed before summing!
        for (int i = 0; i < amount; i++)
        {
            to_send[13 + i / 8] += (uint8_t)(value[i] << (i % 8u));
        }
        status = modbus_send(to_send, 14 + (amount - 1) / 8);
    }
    delete[] to_send;
    return status;
}

/**
 * Read Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of Data to Read
 * @param func      Modbus Functional Code
 */
inline int modbus::modbus_read(uint16_t address, uint16_t amount, int func)
{
    uint8_t to_send[12];
    modbus_build_request(to_send, address, func);
    to_send[5] = 6;
    to_send[10] = (uint8_t)(amount >> 8u);
    to_send[11] = (uint8_t)(amount & 0x00FFu);
    return modbus_send(to_send, 12);
}

/**
 * Read Holding Registers
 * MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 */
inline int modbus::modbus_read_holding_registers(uint16_t address, uint16_t amount, uint16_t *buffer)
{
    if (_connected)
    {
        modbus_read(address, amount, READ_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, READ_REGS);
        if (err)
            return err_no;
        for (auto i = 0; i < amount; i++)
        {
            buffer[i] = ((uint16_t)to_rec[9u + 2u * i]) << 8u;
            buffer[i] += (uint16_t)to_rec[10u + 2u * i];
        }
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Read Input Registers
 * MODBUS FUNCTION 0x04
 * @param address     Reference Address
 * @param amount      Amount of Registers to Read
 * @param buffer      Buffer to Store Data Read from Registers
 */
inline int modbus::modbus_read_input_registers(uint16_t address, uint16_t amount, uint16_t *buffer)
{
    if (_connected)
    {
        modbus_read(address, amount, READ_INPUT_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, READ_INPUT_REGS);
        if (err)
            return err_no;
        for (auto i = 0; i < amount; i++)
        {
            buffer[i] = ((uint16_t)to_rec[9u + 2u * i]) << 8u;
            buffer[i] += (uint16_t)to_rec[10u + 2u * i];
        }
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Read Coils
 * MODBUS FUNCTION 0x01
 * @param address     Reference Address
 * @param amount      Amount of Coils to Read
 * @param buffer      Buffer to Store Data Read from Coils
 */
inline int modbus::modbus_read_coils(uint16_t address, uint16_t amount, bool *buffer)
{
    if (_connected)
    {
        if (amount > 2040)
        {
            set_bad_input();
            return EX_BAD_DATA;
        }
        modbus_read(address, amount, READ_COILS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, READ_COILS);
        if (err)
            return err_no;
        for (auto i = 0; i < amount; i++)
        {
            buffer[i] = (bool)((to_rec[9u + i / 8u] >> (i % 8u)) & 1u);
        }
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Read Input Bits(Discrete Data)
 * MODBUS FUNCITON 0x02
 * @param address   Reference Address
 * @param amount    Amount of Bits to Read
 * @param buffer    Buffer to store Data Read from Input Bits
 */
inline int modbus::modbus_read_input_bits(uint16_t address, uint16_t amount, bool *buffer)
{
    if (_connected)
    {
        if (amount > 2040)
        {
            set_bad_input();
            return EX_BAD_DATA;
        }
        modbus_read(address, amount, READ_INPUT_BITS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        if (err)
            return err_no;
        for (auto i = 0; i < amount; i++)
        {
            buffer[i] = (bool)((to_rec[9u + i / 8u] >> (i % 8u)) & 1u);
        }
        modbuserror_handle(to_rec, READ_INPUT_BITS);
        return 0;
    }
    else
    {
        return BAD_CON;
    }
}

/**
 * Write Single Coils
 * MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 */
inline int modbus::modbus_write_coil(uint16_t address, const bool &to_write)
{
    if (_connected)
    {
        int value = to_write * 0xFF00;
        modbus_write(address, 1, WRITE_COIL, (uint16_t *)&value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_COIL);
        if (err)
            return err_no;
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Write Single Register
 * FUCTION 0x06
 * @param address   Reference Address
 * @param value     Value to Be Written to Register
 */
inline int modbus::modbus_write_register(uint16_t address, const uint16_t &value)
{
    if (_connected)
    {
        modbus_write(address, 1, WRITE_REG, &value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_COIL);
        if (err)
            return err_no;
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Write Multiple Coils
 * MODBUS FUNCTION 0x0F
 * @param address  Reference Address
 * @param amount   Amount of Coils to Write
 * @param value    Values to Be Written to Coils
 */
inline int modbus::modbus_write_coils(uint16_t address, uint16_t amount, const bool *value)
{
    if (_connected)
    {
        uint16_t *temp = new uint16_t[amount];
        for (int i = 0; i < amount; i++)
        {
            temp[i] = (uint16_t)value[i];
        }
        modbus_write(address, amount, WRITE_COILS, temp);
        delete[] temp;
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_COILS);
        if (err)
            return err_no;
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Write Multiple Registers
 * MODBUS FUNCION 0x10
 * @param address Reference Address
 * @param amount  Amount of Value to Write
 * @param value   Values to Be Written to the Registers
 */
inline int modbus::modbus_write_registers(uint16_t address, uint16_t amount, const uint16_t *value)
{
    if (_connected)
    {
        modbus_write(address, amount, WRITE_REGS, value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1)
        {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_REGS);
        if (err)
            return err_no;
        return 0;
    }
    else
    {
        set_bad_con();
        return BAD_CON;
    }
}

/**
 * Data Sender
 * @param to_send Request to Be Sent to Server
 * @param length  Length of the Request
 * @return        Size of the request
 */
inline ssize_t modbus::modbus_send(uint8_t *to_send, size_t length)
{
    _msg_id++;
    return send(_socket, (const char *)to_send, (size_t)length, 0);
}

/**
 * Data Receiver
 * @param buffer Buffer to Store the Data Retrieved
 * @return       Size of Incoming Data
 */
inline ssize_t modbus::modbus_receive(uint8_t *buffer) const
{
    return recv(_socket, (char *)buffer, MAX_MSG_LENGTH, 0);
}

inline void modbus::set_bad_con()
{
    err = true;
    error_msg = "BAD CONNECTION";
}

inline void modbus::set_bad_input()
{
    err = true;
    error_msg = "BAD FUNCTION INPUT";
}

/**
 * Error Code Handler
 * @param msg   Message Received from the Server
 * @param func  Modbus Functional Code
 */
inline void modbus::modbuserror_handle(const uint8_t *msg, int func)
{
    err = false;
    error_msg = "NO ERR";
    if (msg[7] == func + 0x80)
    {
        err = true;
        switch (msg[8])
        {
        case EX_ILLEGAL_FUNCTION:
            error_msg = "1 Illegal Function";
            break;
        case EX_ILLEGAL_ADDRESS:
            error_msg = "2 Illegal Address";
            break;
        case EX_ILLEGAL_VALUE:
            error_msg = "3 Illegal Value";
            break;
        case EX_SERVER_FAILURE:
            error_msg = "4 Server Failure";
            break;
        case EX_ACKNOWLEDGE:
            error_msg = "5 Acknowledge";
            break;
        case EX_SERVER_BUSY:
            error_msg = "6 Server Busy";
            break;
        case EX_NEGATIVE_ACK:
            error_msg = "7 Negative Acknowledge";
            break;
        case EX_MEM_PARITY_PROB:
            error_msg = "8 Memory Parity Problem";
            break;
        case EX_GATEWAY_PROBLEMP:
            error_msg = "10 Gateway Path Unavailable";
            break;
        case EX_GATEWAY_PROBLEMF:
            error_msg = "11 Gateway Target Device Failed to Respond";
            break;
        default:
            error_msg = "UNK";
            break;
        }
    }
}

#endif //MODBUSPP_MODBUS_H
