#include <sys/socket.h>
#include <iostream>
#include "data_protocol.h"
#include "API_CALLS.h"
#include "watchlib.h"
#include "socket_op.h"

using namespace watches;
using namespace IO;

int main(){
	watchlib lib_obj;
	lib_obj.init();

	socket_op s_op;
	sptr<IO::socket> sock = s_op.create(AF_INET, SOCK_STREAM, 0);
	s_op.bind(sock, 1337);
	s_op.listen(sock);
	sptr<IO::socket> cli = s_op.accept(sock);
	char c;
	do{
		std::string data;
		data_protocol::recv(s_op, cli, data);
		c = data[0];
		std::cout<<data<<" "<<c<<'\n';
		lib_obj.send(0, API_CALL::UI_key_pressed, { data });
	}while(c != 'q');
	try{
		s_op.close(cli);
		s_op.close(sock);
		s_op.shutdown(sock);
	}catch(const std::runtime_error &e){
		std::cout<<"error: "<<e.what();
	}
}
