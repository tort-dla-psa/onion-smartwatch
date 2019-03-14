#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include "API_CALLS.h"
#include "data_protocol.h"
#include "socket_op.h"

using namespace watches;
using namespace IO;

int main(){
	socket_op s_op;
	sptr<IO::socket> sock;
	try{
		sock = s_op.create(AF_INET, SOCK_STREAM, 0);
		s_op.connect(sock, "127.0.0.1", 1337);
	}catch(const std::runtime_error &e){
		std::cerr << e.what();
		return -1;
	}
	char c;
	do{
		std::cin >> c;
		data_protocol::send(s_op, sock, std::string(1, c));
	}while(c != 'q');
	s_op.close(sock);
}
