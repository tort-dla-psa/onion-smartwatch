#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include "API_CALLS.h"
#include "data_protocol.h"
#include "socket_op.h"

using namespace watches;
using namespace IO;

int main(const int argc, char* argv[]){
	socket_op s_op;
	sptr<IO::socket> sock;
	try{
		sock = s_op.create(AF_INET, SOCK_STREAM, 0);
		s_op.connect(sock, argv[1], 1337);
	}catch(const std::runtime_error &e){
		std::cerr << e.what();
		return -1;
	}catch(...){
		std::cerr << "some error occured\n";
		return -2;
	}
	char c;
	do{
		std::cin >> c;
		data_protocol::send(s_op, sock, std::string(1, c));
	}while(c != 'q');
	s_op.close(sock);
}
