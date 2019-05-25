#include <sys/socket.h>
#include <iostream>
#include <memory>
#include <string>
#include "data_protocol.h"
#include "API_CALLS.h"
#include "watchlib.h"
#include "socket_op.h"
#include "concurrent_queue.h"

using namespace watches;
using namespace IO;

const std::string exit_cmd = "q";
std::shared_ptr<watchlib> lib_ptr;

void lib_func(){
	lib_ptr = std::make_shared<watchlib>("companion_server");
	lib_ptr->init();
	lib_ptr->start();
}

int main(){
	//weird bug when passing "..." or pre-defined string
	//lib_ptr = std::make_shared<watchlib>(std::string("companion_server"));

	std::thread lib_thread(lib_func);

	socket_op s_op;
	auto sock = s_op.create(AF_INET, SOCK_STREAM, 0);
	s_op.bind(sock, 1337);
	s_op.listen(sock);
	auto cli = s_op.accept(sock);

	while(true){
		std::string data;
		data_protocol::recv(s_op, cli, data);
		std::cout<<"i got:"<<data<<'\n';
		try{
			lib_ptr->send("interface", API_CALL::UI_key_pressed, { data });
			//lib_ptr->send_log("key recieved:"+data, API_CALL::LOG_send_info);
		}catch(const std::runtime_error &e){
			std::cerr<<"error occured:"<<e.what()<<'\n';
		}
		if(data == exit_cmd){
			lib_ptr->end();
			break;
		}
	}

	try{
		s_op.close(cli);
		s_op.close(sock);
	}catch(const std::runtime_error &e){
		std::cout<<"error: "<<e.what();
	}
	if(lib_thread.joinable()){
		lib_thread.join();
	}
}
