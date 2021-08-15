//
// Created by Fanzhe on 5/28/2017.
//

#ifndef MODBUSPP_MODBUS_H
#define MODBUSPP_MODBUS_H

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MAX_MSG_LENGTH 260

///Function Code
#define     READ_COILS        0x01
#define     READ_INPUT_BITS   0x02
#define     READ_REGS         0x03
#define     READ_INPUT_REGS   0x04
#define     WRITE_COIL        0x05
#define     WRITE_REG         0x06
#define     WRITE_COILS       0x0F
#define     WRITE_REGS        0x10

///Exception Codes

#define    EX_ILLEGAL_FUNCTION  0x01 // Function Code not Supported
#define    EX_ILLEGAL_ADDRESS   0x02 // Output Address not exists
#define    EX_ILLEGAL_VALUE     0x03 // Output Value not in Range
#define    EX_SERVER_FAILURE    0x04 // Slave Deive Fails to process request
#define    EX_ACKNOWLEDGE       0x05 // Service Need Long Time to Execute
#define    EX_SERVER_BUSY       0x06 // Server Was Unable to Accept MB Request PDU
#define    EX_NEGATIVE_ACK      0x07
#define    EX_MEM_PARITY_PROB   0x08
#define    EX_GATEWAY_PROBLEMP  0x0A // Gateway Path not Available
#define    EX_GATEWYA_PROBLEMF  0x0B // Target Device Failed to Response
#define    EX_BAD_DATA          0XFF // Bad Data lenght or Address

#define    BAD_CON              -1

/// Modbus Operator Class
/**
 * Modbus Operator Class
 * Providing networking support and mobus operation support.
 */
class modbus {
private:
    bool _connected{};
    uint16_t PORT{};
    int _socket{};
    uint _msg_id{};
    int _slaveid{};
    std::string HOST;

    struct sockaddr_in _server{};


    inline void modbus_build_request(uint8_t *to_send, uint16_t address, int func) const;

    int modbus_read(uint16_t address, uint16_t amount, int func);
    int modbus_write(uint16_t address, uint16_t amount, int func, const uint16_t *value);

    inline ssize_t modbus_send(uint8_t *to_send, size_t length);
    inline ssize_t modbus_receive(uint8_t *buffer) const;

    void modbuserror_handle(const uint8_t *msg, int func);

    inline void set_bad_con();
    inline void set_bad_input();


public:
    bool err{};
    int err_no{};
    std::string error_msg;




    modbus(std::string host, uint16_t port);
    ~modbus();

    bool modbus_connect();
    void modbus_close() const;

    void modbus_set_slave_id(int id);

    int  modbus_read_coils(uint16_t address, uint16_t amount, bool* buffer);
    int  modbus_read_input_bits(uint16_t address, uint16_t amount, bool* buffer);
    int  modbus_read_holding_registers(uint16_t address, uint16_t amount, uint16_t *buffer);
    int  modbus_read_input_registers(uint16_t address, uint16_t amount, uint16_t *buffer);

    int  modbus_write_coil(uint16_t address, const bool& to_write);
    int  modbus_write_register(uint16_t address, const uint16_t& value);
    int  modbus_write_coils(uint16_t address, uint16_t amount, const bool *value);
    int  modbus_write_registers(uint16_t address, uint16_t amount, const uint16_t *value);


};


/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */
modbus::modbus(std::string host, uint16_t port=502) {
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
modbus::~modbus(void) = default;


/**
 * Modbus Slave ID Setter
 * @param id  ID of the Modbus Server Slave
 */
void modbus::modbus_set_slave_id(int id) {
    _slaveid = id;
}



/**
 * Build up a Modbus/TCP Connection
 * @return   If A Connection Is Successfully Built
 */
bool modbus::modbus_connect() {
    if(HOST.empty() || PORT == 0) {
        std::cout << "Missing Host and Port" << std::endl;
        return false;
    } else {
        std::cout << "Found Proper Host "<< HOST << " and Port " <<PORT <<std::endl;
    }

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        std::cout <<"Error Opening Socket" <<std::endl;
        return false;
    } else {
        std::cout <<"Socket Opened Successfully" << std::endl;
    }

    struct timeval timeout{};
    timeout.tv_sec  = 20;  // after 20 seconds connect() will timeout
    timeout.tv_usec = 0;
    setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = inet_addr(HOST.c_str());
    _server.sin_port = htons(PORT);

    if (connect(_socket, (struct sockaddr*)&_server, sizeof(_server)) < 0) {
        std::cout<< "Connection Error" << std::endl;
        return false;
    }

    std::cout<< "Connected" <<std::endl;
    _connected = true;
    return true;
}


/**
 * Close the Modbus/TCP Connection
 */
void modbus::modbus_close() const {
    close(_socket);
    std::cout <<"Socket Closed" <<std::endl;
}


/**
 * Modbus Request Builder
 * @param to_send   Message Buffer to Be Sent
 * @param address   Reference Address
 * @param func      Modbus Functional Code
 */
void modbus::modbus_build_request(uint8_t *to_send, uint16_t address, int func) const {
    to_send[0] = (uint8_t) _msg_id >> 8u;
    to_send[1] = (uint8_t) (_msg_id & 0x00FFu);
    to_send[2] = 0;
    to_send[3] = 0;
    to_send[4] = 0;
    to_send[6] = (uint8_t) _slaveid;
    to_send[7] = (uint8_t) func;
    to_send[8] = (uint8_t) (address >> 8u);
    to_send[9] = (uint8_t) (address & 0x00FFu);
}


/**
 * Write Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of data to be Written
 * @param func      Modbus Functional Code
 * @param value     Data to Be Written
 */
int modbus::modbus_write(uint16_t address, uint16_t amount, int func, const uint16_t *value) {
    int status = 0;
    if(func == WRITE_COIL || func == WRITE_REG) {
        uint8_t to_send[12];
        modbus_build_request(to_send, address, func);
        to_send[5] = 6;
        to_send[10] = (uint8_t) (value[0] >> 8u);
        to_send[11] = (uint8_t) (value[0] & 0x00FFu);
        status = modbus_send(to_send, 12);
    } else if(func == WRITE_REGS){
        uint8_t to_send[13 + 2 * amount];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t) (7 + 2 * amount);
        to_send[10] = (uint8_t) (amount >> 8u);
        to_send[11] = (uint8_t) (amount & 0x00FFu);
        to_send[12] = (uint8_t) (2 * amount);
        for(int i = 0; i < amount; i++) {
            to_send[13 + 2 * i] = (uint8_t) (value[i] >> 8u);
            to_send[14 + 2 * i] = (uint8_t) (value[i] & 0x00FFu);
        }
        status = modbus_send(to_send, 13 + 2 * amount);
    } else if(func == WRITE_COILS) {
        uint8_t to_send[14 + (amount -1) / 8 ];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t) (7 + (amount + 7) / 8);
        to_send[10] = (uint8_t) (amount >> 8u);
        to_send[11] = (uint8_t) (amount & 0x00FFu);
        to_send[12] = (uint8_t) ((amount + 7) / 8);
        for(int i = 0; i < (amount+7)/8; i++)
            to_send[13 + i] = 0; // init needed before summing!
        for(int i = 0; i < amount; i++) {
            to_send[13 + i/8] += (uint8_t) (value[i] << (i % 8u));
        }
        status = modbus_send(to_send, 14 + (amount - 1) / 8);
    }
    return status;

}


/**
 * Read Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of Data to Read
 * @param func      Modbus Functional Code
 */
int modbus::modbus_read(uint16_t address, uint16_t amount, int func){
    uint8_t to_send[12];
    modbus_build_request(to_send, address, func);
    to_send[5] = 6;
    to_send[10] = (uint8_t) (amount >> 8u);
    to_send[11] = (uint8_t) (amount & 0x00FFu);
    return modbus_send(to_send, 12);
}


/**
 * Read Holding Registers
 * MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 */
int modbus::modbus_read_holding_registers(uint16_t address, uint16_t amount, uint16_t *buffer) {
    if(_connected) {
        modbus_read(address, amount, READ_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, READ_REGS);
        if(err) return err_no;
        for(uint i = 0; i < amount; i++) {
            buffer[i] = ((uint16_t)to_rec[9u + 2u * i]) << 8u;
            buffer[i] += (uint16_t) to_rec[10u + 2u * i];
        }
        return 0;
    } else {
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
int modbus::modbus_read_input_registers(uint16_t address, uint16_t amount, uint16_t *buffer) {
    if(_connected){
        modbus_read(address, amount, READ_INPUT_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, READ_INPUT_REGS);
        if(err) return err_no;
        for(uint i = 0; i < amount; i++) {
            buffer[i] = ((uint16_t)to_rec[9u + 2u * i]) << 8u;
            buffer[i] += (uint16_t) to_rec[10u + 2u * i];
        }
        return 0;
    } else {
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
int modbus::modbus_read_coils(uint16_t address, uint16_t amount, bool *buffer) {
    if(_connected) {
        if(amount > 2040) {
            set_bad_input();
            return EX_BAD_DATA;
        }
        modbus_read(address, amount, READ_COILS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, READ_COILS);
        if(err) return err_no;
        for(uint i = 0; i < amount; i++) {
            buffer[i] = (bool) ((to_rec[9u + i / 8u] >> (i % 8u)) & 1u);
        }
        return 0;
    } else {
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
int modbus::modbus_read_input_bits(uint16_t address, uint16_t amount, bool* buffer) {
    if(_connected) {
        if(amount > 2040) {
            set_bad_input();
            return EX_BAD_DATA;
        }
        modbus_read(address, amount, READ_INPUT_BITS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        if(err) return err_no;
        for(uint i = 0; i < amount; i++) {
            buffer[i] = (bool) ((to_rec[9u + i / 8u] >> (i % 8u)) & 1u);
        }
        modbuserror_handle(to_rec, READ_INPUT_BITS);
        return 0;
    } else {
        return BAD_CON;
    }
}


/**
 * Write Single Coils
 * MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 */
int modbus::modbus_write_coil(uint16_t address, const bool& to_write) {
    if(_connected) {
        int value = to_write * 0xFF00;
        modbus_write(address, 1, WRITE_COIL, (uint16_t *)&value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_COIL);
        if(err) return err_no;
        return 0;
    } else {
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
int modbus::modbus_write_register(uint16_t address, const uint16_t& value) {
    if(_connected) {
        modbus_write(address, 1, WRITE_REG, &value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_COIL);
        if(err) return err_no;
        return 0;
    } else {
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
int modbus::modbus_write_coils(uint16_t address, uint16_t amount, const bool *value) {
    if(_connected) {
        uint16_t temp[amount];
        for(int i = 0; i < amount; i++) {
            temp[i] = (uint16_t)value[i];
        }
        modbus_write(address, amount, WRITE_COILS, temp);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_COILS);
        if(err) return err_no;
        return 0;
    } else {
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
int modbus::modbus_write_registers(uint16_t address, uint16_t amount, const uint16_t *value) {
    if(_connected) {
        modbus_write(address, amount, WRITE_REGS, value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        if (k == -1) {
            set_bad_con();
            return BAD_CON;
        }
        modbuserror_handle(to_rec, WRITE_REGS);
        if(err) return err_no;
        return 0;
    } else {
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
ssize_t modbus::modbus_send(uint8_t *to_send, size_t length) {
    _msg_id++;
    return send(_socket, to_send, (size_t)length, 0);
}


/**
 * Data Receiver
 * @param buffer Buffer to Store the Data Retrieved
 * @return       Size of Incoming Data
 */
ssize_t modbus::modbus_receive(uint8_t *buffer) const {
    return recv(_socket, (char *) buffer, 1024, 0);
}

void modbus::set_bad_con() {
    err = true;
    error_msg = "BAD CONNECTION";
}


void modbus::set_bad_input() {
    err = true;
    error_msg = "BAD FUNCTION INPUT";
}

/**
 * Error Code Handler
 * @param msg   Message Received from the Server
 * @param func  Modbus Functional Code
 */
void modbus::modbuserror_handle(const uint8_t *msg, int func) {
    if(msg[7] == func + 0x80) {
        err = true;
        switch(msg[8]){
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
            case EX_GATEWYA_PROBLEMF:
                error_msg = "11 Gateway Target Device Failed to Respond";
                break;
            default:
                error_msg = "UNK";
                break;
        }
    }
    err = false;
    error_msg = "NO ERR";
}

#endif //MODBUSPP_MODBUS_H
