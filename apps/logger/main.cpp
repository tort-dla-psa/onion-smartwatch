#include <iostream>
#include "file.h"
#include "file_op.h"
#include "watchlib.h"

using namespace watches;
using namespace IO;

void cb_info(packet p){

}
void cb_debug(packet p){

}
void cb_error(packet p){

}

int main(const int argc, char* argv[]){
	watchlib lib_obj;
	lib_obj.add_callback(API_CALL::LOG_send_info, cb_info);
	lib_obj.add_callback(API_CALL::LOG_send_debug, cb_debug);
	lib_obj.add_callback(API_CALL::LOG_send_error, cb_error);
}
