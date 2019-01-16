//
// Created by Fanzhe on 5/28/2017

#include "modbus.h"

#if COM_PLATFORM == COM_FOR_LINUX
#include <unistd.h>
#include <arpa/inet.h>
#endif

using namespace std;


/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */
modbus::modbus(string host, uint16_t port) {
#if COM_PLATFORM == COM_FOR_WIN
	WSADATA w;
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		exit(0);
	}
#endif
	HOST = host;
	PORT = port;
	_slaveid = 1;
	_msg_id = 1;
	_connected = false;
}


/**
 * Overloading Modbus Connector Constructor with Default Port Set at 502
 * @param host  IP Address of Host
 * @return      A Modbus Connector Object
 */
modbus::modbus(string host) {
#if COM_PLATFORM == COM_FOR_WIN
	WSADATA w;
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		fprintf(stderr, "Could not open Windows connection.\n");
		exit(0);
	}
#endif
    modbus(host, 502);
}


/**
 * Destructor of Modbus Connector Object
 */
modbus::~modbus(void) {
	freeaddrinfo(f_addrinfo);
#if COM_PLATFORM == COM_FOR_WIN
	closesocket(_socket);
	WSACleanup();
	exit(0);
#elif COM_PLATFORM == COM_FOR_LINUX
	close(f_socket);
#endif
}


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

    if(HOST == "" || PORT == 0) {
        std::cout << "Missing Host and Port" << std::endl;
        return false;
    } else {
        std::cout << "Found Proper Host " << HOST << " and Port " << PORT << std::endl;
    }

	char decimal_port[16];
	snprintf(decimal_port, sizeof(decimal_port), "%d", PORT);
	decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';

	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;//AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;//SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_TCP;
	int r(getaddrinfo(HOST.c_str(), decimal_port, &hints, &f_addrinfo));
	if (r != 0 || f_addrinfo == NULL)
	{
		std::cout << "invalid address or port for UDP socket: \"" << HOST << ":" << decimal_port << "\"" << std::endl;
	}
#if COM_PLATFORM == COM_FOR_WIN
	_socket = socket(f_addrinfo->ai_family, SOCK_STREAM, IPPROTO_TCP);
#elif COM_PLATFORM == COM_FOR_LINUX
_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
#endif

    //_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1) {
        std::cout <<"Error Opening Socket" << std::endl;
#if COM_PLATFORM == COM_FOR_WIN
		WSACleanup();
#endif
        return false;
    } else {
        std::cout <<"Socket Opened Successfully" << std::endl;
    }

	r = connect(_socket, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen);
	if (r != 0)
	{
		freeaddrinfo(f_addrinfo);
#if COM_PLATFORM == COM_FOR_LINUX
		close(_socket);
		std::cout << "Linux: could not bind UDP socket with: \"" << HOST << ":" << decimal_port << "\"" << std::endl;
#elif COM_PLATFORM == COM_FOR_WIN
		closesocket(_socket);
		std::cout << "Windows: could not bind UDP socket with: \"" << HOST << ":" << decimal_port << "\"" << std::endl;
		WSACleanup();
#endif
		return false;
	}

	std::cout << "Connected" << std::endl;
	_connected = true;
	return true;




}


/**
 * Close the Modbus/TCP Connection
 */
void modbus::modbus_close() {
#if COM_PLATFORM == COM_FOR_LINUX
	close(_socket);
#elif COM_PLATFORM == COM_FOR_WIN
	closesocket(_socket);
#endif
    std::cout <<"Socket Closed" <<std::endl;
}


/**
 * Modbus Request Builder
 * @param to_send   Message Buffer to Be Sent
 * @param address   Reference Address
 * @param func      Modbus Functional Code
 */
void modbus::modbus_build_request(uint8_t *to_send, int address, int func) {
    to_send[0] = (uint8_t) _msg_id >> 8;
    to_send[1] = (uint8_t) (_msg_id & 0x00FF);
    to_send[2] = 0;
    to_send[3] = 0;
    to_send[4] = 0;
    to_send[6] = (uint8_t) _slaveid;
    to_send[7] = (uint8_t) func;
    to_send[8] = (uint8_t) (address >> 8);
    to_send[9] = (uint8_t) (address & 0x00FF);
}


/**
 * Write Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of data to be Written
 * @param func      Modbus Functional Code
 * @param value     Data to Be Written
 */
void modbus::modbus_write(int address, int amount, int func, uint16_t *value) {
    if(func == WRITE_COIL || func == WRITE_REG) {
        uint8_t* to_send = new uint8_t[12];
        modbus_build_request(to_send, address, func);
        to_send[5] = 6;
        to_send[10] = (uint8_t) (value[0] >> 8);
        to_send[11] = (uint8_t) (value[0] & 0x00FF);
		printf("Write single coils or single reg function code 5 or 6 \n");
        modbus_send(to_send, 12);
		
		//for (int j = 0; j < 12;j++)
			//printf("%0X\n", to_send[j]);
		//printf("\n");
    } else if(func == WRITE_REGS){
        uint8_t* to_send = new uint8_t[13 + 2 * amount]; // updated by FM, uint8_t to_send[13 + 2 * amount];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t) (5 + 2 * amount);
        to_send[10] = (uint8_t) (amount >> 8);
        to_send[11] = (uint8_t) (amount & 0x00FF);
        to_send[12] = (uint8_t) (2 * amount);
        for(int i = 0; i < amount; i++) {
            to_send[13 + 2 * i] = (uint8_t) (value[i] >> 8);
            to_send[14 + 2 * i] = (uint8_t) (value[i] & 0x00FF);
        }
		printf("Write registers function code 10=16\n");
        modbus_send(to_send, 13 + 2 * amount);
		
		//for (int j = 0; j < 13 + 2 * amount;j++)
			//printf("%0X\n", to_send[j]);
		//printf("\n");
    } else if(func == WRITE_COILS) {
        uint8_t * to_send = new uint8_t[14 + (amount -1) / 8 ];// updated by FM, uint8_t to_send[14 + (amount -1) / 8];
		memset(to_send, 0, 14 + (amount - 1) / 8);
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t) (7 + (amount -1) / 8);
        to_send[10] = (uint8_t) (amount >> 8);
        to_send[11] = (uint8_t) amount; // updated FM (uint8_t) (amount >> 8);
        to_send[12] = (uint8_t) ((amount + 7) / 8);
        for(int i = 0; i < amount; i++) {
            to_send[13 + (i - 1) / 8] += (uint8_t) (value[i] << (i % 8));
        }
		printf("Write coils function code 0F=15\n");
        modbus_send(to_send, 14 + (amount - 1) / 8);
		
		//for (int j = 0; j < 14 + (amount - 1) / 8;j++)
			//printf("%d = %0X\n",j, to_send[j]);
		//printf("\n");
    }
}


/**
 * Read Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of Data to Read
 * @param func      Modbus Functional Code
 */
void modbus::modbus_read(int address, int amount, int func){
    uint8_t to_send[12];
    modbus_build_request(to_send, address, func);
    to_send[5] = 6;
    to_send[10] = (uint8_t) (amount >> 8);
    to_send[11] = (uint8_t) (amount & 0x00FF);
    modbus_send(to_send, 12);
}


/**
 * Read Holding Registers
 * MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 */
void modbus::modbus_read_holding_registers(int address, int amount, uint16_t *buffer) {
    if(_connected) {
        if(amount > 65535 || address > 65535) {
            throw modbus_amount_exception();
        }
        modbus_read(address, amount, READ_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try {
            modbus_error_handle(to_rec, READ_REGS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) to_rec[10 + 2 * i];
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}


/**
 * Read Input Registers 
 * MODBUS FUNCTION 0x04
 * @param address     Reference Address
 * @param amount      Amount of Registers to Read
 * @param buffer      Buffer to Store Data Read from Registers
 */
void modbus::modbus_read_input_registers(int address, int amount, uint16_t *buffer) {
    if(_connected){
        if(amount > 65535 || address > 65535) {
            throw modbus_amount_exception();
        }
        modbus_read(address, amount, READ_INPUT_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try {
            modbus_error_handle(to_rec, READ_INPUT_REGS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) to_rec[10 + 2 * i];
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}


/**
 * Read Coils           
 * MODBUS FUNCTION 0x01
 * @param address     Reference Address
 * @param amount      Amount of Coils to Read
 * @param buffer      Buffer to Store Data Read from Coils
 */
void modbus::modbus_read_coils(int address, int amount, bool *buffer) {
	try // Added by FM: try catch block, please correct all other functions
	{
		if (_connected) {
			if (amount > 2040 || address > 65535) {
				throw modbus_amount_exception();
			}
			modbus_read(address, amount, READ_COILS);
			uint8_t to_rec[MAX_MSG_LENGTH];
			ssize_t k = modbus_receive(to_rec);
			try {
				modbus_error_handle(to_rec, READ_COILS);
				for (int i = 0; i < amount; i++) {
					buffer[i] = (bool)((to_rec[9 + i / 8] >> (i % 8)) & 1);
				}
			}
			catch (exception &e) {
				cout << e.what() << endl;
				delete(&to_rec);
				delete(&k);
				throw e;
			}
		}
		else {
			throw modbus_connect_exception();
		}
	} // end try
	catch (exception &e)
	{
		cout << e.what() << endl;
	}
}


/**
 * Read Input Bits(Discrete Data)
 * MODBUS FUNCITON 0x02
 * @param address   Reference Address
 * @param amount    Amount of Bits to Read
 * @param buffer    Buffer to store Data Read from Input Bits
 */
void modbus::modbus_read_input_bits(int address, int amount, bool* buffer) {
    if(_connected) {
        if(amount > 2040 || address > 65535) {
            throw modbus_amount_exception();
        }
        modbus_read(address, amount, READ_INPUT_BITS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try {
            modbus_error_handle(to_rec, READ_INPUT_BITS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
            }
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}


/**
 * Write Single Coils
 * MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 */
void modbus::modbus_write_coil(int address, bool to_write) {
    if(_connected) {
        if(address > 65535) {
            throw modbus_amount_exception();
        }
        int value = to_write * 0xFF00;
        modbus_write(address, 1, WRITE_COIL, (uint16_t *)&value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try{
            modbus_error_handle(to_rec, WRITE_COIL);
        } catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}


/**
 * Write Single Register
 * FUCTION 0x06
 * @param address   Reference Address
 * @param value     Value to Be Written to Register
 */
void modbus::modbus_write_register(int address, uint16_t value) {
    if(_connected) {
        if(address > 65535) {
            throw modbus_amount_exception();
        }
        modbus_write(address, 1, WRITE_REG, &value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try{
            modbus_error_handle(to_rec, WRITE_COIL);
        } catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}


/**
 * Write Multiple Coils 
 * MODBUS FUNCTION 0x0F
 * @param address  Reference Address
 * @param amount   Amount of Coils to Write
 * @param value    Values to Be Written to Coils
 */
void modbus::modbus_write_coils(int address, int amount, bool *value) {
    if(_connected) {
        if(address > 65535 || amount > 65535) {
            throw modbus_amount_exception();
        }
        uint16_t* temp = new uint16_t[amount];
        for(int i = 0; i < amount; i++) { // updated by FM for(int i = 0; i < 4; i++) {
            temp[i] = (uint16_t)value[i];
        }
        modbus_write(address, amount, WRITE_COILS,  temp);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try{
            modbus_error_handle(to_rec, WRITE_COILS);
        } catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}


/**
 * Write Multiple Registers 
 * MODBUS FUNCION 0x10
 * @param address Reference Address
 * @param amount  Amount of Value to Write
 * @param value   Values to Be Written to the Registers
 */
void modbus::modbus_write_registers(int address, int amount, uint16_t *value) {
    if(_connected) {
        if(address > 65535 || amount > 65535) {
            throw modbus_amount_exception();
        }
        modbus_write(address, amount, WRITE_REGS, value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        try{
            modbus_error_handle(to_rec, WRITE_REGS);
        } catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }
    } else {
        throw modbus_connect_exception();
    }
}

void print_to_send(uint8_t *to_send, int length)
{
	printf("modbus_send\n");
	for (int j = 0; j < length;j++)
	{
		if (j == 6)
			printf("Slave Address: ");
		if (j == 7)
			printf("Function code: ");
		if (j == 8)
			printf("Start add HI : ");
		if (j == 9)
			printf("Start add LO : ");
		if (j == 10)
			printf("No.Points HI : ");
		if (j == 11)
			printf("No.Points LO : ");
		if (j == 12)
			printf("Byte count   : ");
		if (j == 13)
			printf("Force data HI: ");
		if (j == 14)
			printf("Force data LO: ");
		printf("%d = %0X\n", j, to_send[j]);
	}
	printf("\n");
}

/**
 * Data Sender
 * @param to_send Request to Be Sent to Server
 * @param length  Length of the Request
 * @return        Size of the request
 */
ssize_t modbus::modbus_send(uint8_t *to_send, int length) {
    _msg_id++;
	print_to_send(to_send, length);
    return send(_socket, (const char*) to_send, (size_t)length, 0); // FM update with: (const char*) 
}


/**
 * Data Receiver
 * @param buffer Buffer to Store the Data Retrieved
 * @return       Size of Incoming Data
 */
ssize_t modbus::modbus_receive(uint8_t *buffer) {
    return recv(_socket, (char *) buffer, 1024, 0);
}


/**
 * Error Code Handler
 * @param msg   Message Received from the Server
 * @param func  Modbus Functional Code
 */
void modbus::modbus_error_handle(uint8_t *msg, int func) {
    if(msg[7] == func + 0x80) {
        switch(msg[8]){
            case EX_ILLEGAL_FUNCTION:
                throw modbus_illegal_function_exception();
            case EX_ILLEGAL_ADDRESS:
                throw modbus_illegal_address_exception();
            case EX_ILLEGAL_VALUE:
                throw modbus_illegal_data_value_exception();
            case EX_SERVER_FAILURE:
                throw modbus_server_failure_exception();
            case EX_ACKNOWLEDGE:
                throw modbus_acknowledge_exception();
            case EX_SERVER_BUSY:
                throw modbus_server_busy_exception();
            case EX_GATEWAY_PROBLEMP:
            case EX_GATEWYA_PROBLEMF:
                throw modbus_gateway_exception();
            default:
                break;
        }
    }
}
