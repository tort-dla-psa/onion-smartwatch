#include <iostream>
#include <thread> //sleep
#include "API_CALLS.h" //API
#include "watchlib.h" //lib, obviously

using namespace watches;
using namespace IO;

bool end_requested = false;

void cb_info(const packet &p){
	std::cout<<"[info]:"<<p.get_name()<<":"<<p.get_args()[0]<<"\n";
}
void cb_debug(const packet &p){
	std::cout<<"[debug]:"<<p.get_name()<<":"<<p.get_args()[0]<<"\n";
}
void cb_error(const packet &p){
	std::cout<<"[error]:"<<p.get_name()<<":"<<p.get_args()[0]<<"\n";
}
void cb_end(const packet &p){
	std::cout<<"end requested, ending\n";
	end_requested = true;
}

int main(const int argc, char* argv[]){
	watchlib lib_obj("logger");
	lib_obj.init();
	lib_obj.add_callback(API_CALL::LOG_send_info, cb_info);
	lib_obj.add_callback(API_CALL::LOG_send_debug, cb_debug);
	lib_obj.add_callback(API_CALL::LOG_send_error, cb_error);
	lib_obj.add_callback(API_CALL::request_end, cb_end);
	lib_obj.start();
}
