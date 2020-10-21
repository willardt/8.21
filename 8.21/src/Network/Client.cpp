#include "Client.h"

#include <iostream>
#include <cassert>

#include "Packet.h"

#define DEFAULT_PORT "23001"
#define DEFAULT_HOST "192.168.1.4"

Client::Client() :
	_id					( -1 ),
	_started			( false ),
	_connect_socket		( INVALID_SOCKET )
{
	load_client_commands();
}

Client::~Client() {
	closesocket(_connect_socket);

	if (_started) {
		WSACleanup();
		if (_recieve_thread.joinable()) {
			_recieve_thread.detach();
		}
	}
}

void Client::c_startup() {
	int r_startup = WSAStartup(MAKEWORD(2, 2), &_wsa_data);
	std::cout << "WsaStartup -- " << r_startup << '\n';
	_started = true;

	addrinfo* result, * ptr, hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int r_addrinfo = getaddrinfo(DEFAULT_HOST, DEFAULT_PORT, &hints, &result);
	std::cout << "getaddrinfo -- " << r_addrinfo << '\n';

	for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
		_connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (_connect_socket == INVALID_SOCKET) {
			std::cout << "Socket Error: " << WSAGetLastError() << '\n';
			continue;
		}

		break;
	}

	assert(ptr);

	int r_connect = connect(_connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (r_connect == SOCKET_ERROR) {
		std::cout << "Connect Error : " << WSAGetLastError() << '\n';
	}
	else {
		_recieve_thread = std::thread(&Client::c_recieve, this);
	}
}

bool Client::c_send(const char* data, int* len) {
	assert(*len <= 512);

	int total = 0;
	int bytes_left = *len;
	while (total < bytes_left) {
		int r_send = send(_connect_socket, data + total, bytes_left, 0);
		std::cout << "send Result: " << r_send << '\n';
		if (r_send == SOCKET_ERROR) {
			std::cout << "Send Error: " << WSAGetLastError() << '\n';
			return false;
		}

		total += r_send;
		bytes_left -= r_send;
	}

	*len = total;

	return true;
}

void Client::c_recieve() {
	char recvbuf[1024];
	char* ptr;
	int r_recv = -1;
	int recvbuflen = 512;
	int bytes_handled = 0;
	int remaining_bytes = 0;
	int packet_length = 0;

	do {
		std::cout << "Waiting for server..." << '\n';
		bytes_handled = 0;
		r_recv = recv(_connect_socket, recvbuf + remaining_bytes, recvbuflen, 0);
		if(r_recv > 0) {
			std::cout << "Recieved --- " << r_recv << '\n';
			ptr = recvbuf;

			memcpy(&packet_length, ptr, sizeof(int));
			remaining_bytes = r_recv + remaining_bytes;

			while (remaining_bytes >= packet_length) {
				std::cout << ptr + 4 << '\n';

				const char* key = ptr + 4; // skip int header
				if (_client_commands.find(key) == _client_commands.end()) {
					std::cout << "Unknown Client Command : " << key << '\n';
				}
				else {
					(this->*_client_commands.at(ptr + 4))(static_cast<void*>(ptr + 58), packet_length - 58);
				}

				bytes_handled += packet_length;
				remaining_bytes -= packet_length;

				if (remaining_bytes > 4) {
					ptr = recvbuf + bytes_handled;
					memcpy(&packet_length, ptr, sizeof(int));
				}
			}

			if(remaining_bytes > 0) {
				memcpy(recvbuf, ptr, remaining_bytes);
			}
		}
		else if(r_recv == 0) {
			std::cout << "Close Connection" << '\n';
		}
		else {
			std::cout << "Recv Error : " << WSAGetLastError() << '\n';
		}
	} while (r_recv > 0);
}

bool Client::c_connected() {
	return _connect_socket == INVALID_SOCKET;
}

void Client::load_world_server() {
	PacketData packet("load_world_server", _id);
	int len = packet.length();

	c_send(packet.c_str(), &len);
}

void Client::load_client_commands() {
	_client_commands.emplace("set_id", &Client::set_id);
}

void Client::set_id(void* buf, int size) {
	assert(size == sizeof(int));

	memcpy(&_id, buf, sizeof(int));
}

int Client::get_id() {
	return _id;
}

/********************************************************************************************************************************************************/